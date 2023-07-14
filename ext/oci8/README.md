# The OCI8 Extension

Use the OCI8 extension to access Oracle Database.

Documentation is at https://www.php.net/oci8

Use `pecl install oci8` to install for PHP 8.2.

Use `pecl install oci8-3.2.1` to install for PHP 8.1.

Use `pecl install oci8-3.0.1` to install for PHP 8.0.

Use `pecl install oci8-2.2.0` to install for PHP 7.

The OCI8 extension can be linked with Oracle client libraries from Oracle
Database 11.2 or later.  These libraries are found in your database
installation, or in the free Oracle Instant Client from
https://www.oracle.com/database/technologies/instant-client.html
Install the 'Basic' or 'Basic Light' Instant Client package. If building from
source, then also install the SDK package.

Oracle's standard cross-version connectivity applies.  For example, PHP OCI8
linked with Instant Client 19c can connect to Oracle Database 11.2 onward.  See
Oracle's note "Oracle Client / Server Interoperability Support" (ID 207303.1)
for details.
