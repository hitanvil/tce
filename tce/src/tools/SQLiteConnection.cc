/**
 * @file SQLiteConnection.cc
 *
 * Implementation of SQLiteConnection class.
 *
 * @author Pekka Jääskeläinen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#include <string>
using std::string;

#include "SQLiteConnection.hh"
#include "SQLiteQueryResult.hh"

/**
 * Constructor.
 *
 * @param connection A pointer to a SQLite connection handle.
 */
SQLiteConnection::SQLiteConnection(sqlite3* connection) :
    connection_(connection), transactionActive_(false) {
}

/**
 * Destructor.
 *
 * Closes the connection.
 */
SQLiteConnection::~SQLiteConnection() {
    sqlite3_close(connection_);
}

/**
 * Performs a query that changes the database (UPDATE/INSERT/DELETE).
 *
 * Does not autocommit the changes in case the update is in the middle of
 * transaction.
 *
 * @param queryString The query string.
 * @return Number of rows affected by the change.
 * @exception RelationalDBException In case a database error occured.
 */
int
SQLiteConnection::updateQuery(const std::string& queryString)
    throw (RelationalDBException) {

    if (connection_ == NULL) {
        throw RelationalDBException(
            __FILE__, __LINE__, "SQLiteConnection::updateQuery()",
            "Not connected!");
    }

    // "virtual machine" used by SQLite to execute the statements
    sqlite3_stmt* stmt = compileQuery(queryString);

    assert(stmt != NULL);

    throwIfSQLiteError(sqlite3_step(stmt));
    finalizeQuery(stmt);

    return sqlite3_changes(connection_);
}

/**
 * Performs a SQL Data Definition Language query, that is a query that may
 * change the structure of the database (CREATE TABLE, etc.).
 *
 * Does not autocommit the changes in case the update is in the middle of
 * transaction.
 *
 * @param queryString The query string.
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::DDLQuery(const std::string& queryString)
    throw (RelationalDBException) {
    updateQuery(queryString);
}

/**
 * Performs a data retrieval query (SELECT).
 *
 * @param queryString The query string.
 * @return A handle to the query result set.
 * @exception RelationalDBException In case a database error occured.
 */
RelationalDBQueryResult*
SQLiteConnection::query(const std::string& queryString)
    throw (RelationalDBException) {
    sqlite3_stmt* stmt = compileQuery(queryString);
    SQLiteQueryResult* result = new SQLiteQueryResult(stmt, this);
    return result;
}

/**
 * Starts a new database transaction.
 *
 * Also ends the possible previous transaction successfully (commit).
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::beginTransaction()
    throw (RelationalDBException) {
    if (transactionActive_) {
        commit();
    }
    updateQuery("BEGIN;");
    transactionActive_ = true;
}

/**
 * Ends the current database transaction unsuccessfully and rollbacks all
 * the changes the queries in the transaction would have done.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::rollback()
    throw (RelationalDBException) {
    updateQuery("ROLLBACK;");
    transactionActive_ = false;
}

/**
 * Ends the current database transaction successfully and commits all
 * the changes in the transaction to the database.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::commit()
    throw (RelationalDBException) {
    updateQuery("COMMIT;");
    transactionActive_ = false;
}


/**
 * Returns the row ID of the most recent insert in the database.
 *
 * @return The row ID.
 */
RowID
SQLiteConnection::lastInsertRowID() {
    return sqlite3_last_insert_rowid(connection_);
}


/**
 * Throws a RelationalDBException if result value indicates an SQLite error.
 *
 * @param result The value from a SQLite API call.
 * @exception RelationalDBException Thrown if result is not SQLITE_OK.
 */
void
SQLiteConnection::throwIfSQLiteError(int result)
    throw (RelationalDBException) {
    if (result != SQLITE_OK && result != SQLITE_ROW &&
        result != SQLITE_DONE) {

        string error = sqlite3_errmsg(connection_);
        throw RelationalDBException(__FILE__, __LINE__, "", error);
    }
}

/**
 * Compiles a SQLite query.
 *
 * @param queryString The SQL statement to compile.
 * @return The SQLite virtual machine that should be used to execute the query.
 * @exception RelationalDBException In case a database error occured.
 */
sqlite3_stmt*
SQLiteConnection::compileQuery(const std::string& queryString)
    throw (RelationalDBException) {

    // "virtual machine" used by SQLite to execute the statements
    sqlite3_stmt* stmt = NULL;
    const char* dummy = NULL;

    throwIfSQLiteError(sqlite3_prepare(
        connection_, queryString.c_str(), queryString.length(),
        &stmt, &dummy));
    return stmt;
}

/**
 * Finalizes a SQLite query, frees the virtual machine.
 *
 * Does nothing if the statement is NULL.
 *
 * @param statement The SQLite statement to free.
 * @exception RelationalDBException In case there was errors (can be
 * cumulated from a previous sqlite3_step() call).
 */

void
SQLiteConnection::finalizeQuery(sqlite3_stmt* statement)
    throw (RelationalDBException) {

    if (statement == NULL) {
        return;
    }

    throwIfSQLiteError(sqlite3_finalize(statement));
}
