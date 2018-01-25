<?php
/*
 * File: mysql_users.php
 * Author: Yasuo Ohgaki <yohgaki@php.net>
 *
 * This file contains example user defined functions that does
 * similar to MySQL functions. They can be implemented as module
 * functions, but there won't be many users need them.
 *
 * Requires: PostgreSQL 7.2.x
 */

/*
 * mysql_list_dbs()
 *
 * This function should be needed, since PostgreSQL connection
 * binds database.
 */
function pg_list_dbs($db)
{
        assert(is_resource($db));
        $query = '
SELECT
 d.datname as "Name",
 u.usename as "Owner",
 pg_encoding_to_char(d.encoding) as "Encoding"
FROM
 pg_database d LEFT JOIN pg_user u ON d.datdba = u.usesysid
ORDER BY 1;
';
        return pg_query($db, $query);
}


/*
 * mysql_list_tables()
 */
function pg_list_tables($db)
{
        assert(is_resource($db));
        $query = "
SELECT
 c.relname as \"Name\",
 CASE c.relkind WHEN 'r' THEN 'table' WHEN 'v' THEN 'view' WHEN 'i' THEN 'index' WHEN 'S' THEN 'sequence' WHEN 's' THEN 'special' END as \"Type\",
  u.usename as \"Owner\"
FROM
 pg_class c LEFT JOIN pg_user u ON c.relowner = u.usesysid
WHERE
 c.relkind IN ('r','v','S','')
 AND c.relname !~ '^pg_'
ORDER BY 1;
";
        return pg_query($db, $query);
}

/*
 * mysql_list_fields()
 *
 * See also pg_meta_data(). It returns field definition as array.
 */
function pg_list_fields($db, $table)
{
        assert(is_resource($db));
        $query = "
SELECT
 a.attname,
 format_type(a.atttypid, a.atttypmod),
 a.attnotnull,
 a.atthasdef,
 a.attnum
FROM
 pg_class c,
 pg_attribute a
WHERE
 c.relname = '".$table."'
 AND a.attnum > 0 AND a.attrelid = c.oid
ORDER BY a.attnum;
";
        return pg_query($db, $query);
}

?>
