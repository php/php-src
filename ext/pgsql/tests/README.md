# The pgsql extension tests

Test scripts assumes:

* PostgreSQL server is installed locally
* There is a PostgreSQL account for the users running the test scripts
* There is database named `test`

For instance, if your login name is `testuser`, you should have PostgreSQL user
account named `testuser` and grant that user access to the database `test`.

If you have account and database, type `createdb test` from command prompt to
create the database to execute the test scripts. By executing the above command
as the same user running the tests you ensure that the user is granted access to
the database.

If you find problems in PostgreSQL extension, please report a
[bug](https://github.com/php/php-src/issues).
