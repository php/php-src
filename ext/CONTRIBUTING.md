# READY FOR REVIEW
Memory leak fix and test
# Summary
 - Introduce a limit for the usage of PHP interned strings.
  1. We keep the feature of the interned strings but limit its usage to 1024 times only.
  2. The optimization is for per request applications of PHP and we want to disable it in persisting PHP applications where it fills up the memory.
  3. The limit of 1024 usages of interned string allocations is derived by the heuristic that number of unique words and number of queries in a single request are rarely double digit numbers.
 - Adds unit test to reproduce the bug caused by the feature
  1. By setting the memory limit we are able to reduce the time to reproduce the bug.
  2. Limit of 2M bytes is used because smaller will be reset to 2M by PHP. We want to force the error to occur sooner.
  3. register_shutdown_function is used to detect the problem when interned memory overflow causes a session to shut down. In this case error_get_last() reports an error from the Zend engine like this one: "Allowed memory size of 2097152 bytes exhausted at Zend\zend_hash.c:1186 (tried to allocate 655392 bytes)"
# Criticality
- 0 for per request usage with web server
- 5 for persistent stand alone usage of PHP with mysqlnd
# Steps to Test
1. Introduce memory_limit up to 2 MB
2. Run 20,000 queries that have at least one different word from each other
3. Then PHP interned strings usage in mysqlnd fills up the memory
# Affected Projects or Products
- All extension that use mysqlnd
# Associated Issues and/or People
https://bugs.php.net/bug.php?id=79163