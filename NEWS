PHP                                                                        NEWS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
04 Mar 2021, PHP 8.0.3

- Core:
  . Fixed #80706 (mail(): Headers after Bcc headers may be ignored). (cmb)

- DOM:
  . Fixed bug #80600 (DOMChildNode::remove() doesn't work on CharacterData
    nodes). (beberlei)

- Gettext:
  . Fixed bug #53251 (bindtextdomain with null dir doesn't return old value).
    (cmb)

- MySQLnd:
  . Fixed bug #78680 (mysqlnd's mysql_clear_password does not transmit
    null-terminated password). (Daniel Black)
  . Fixed bug #80713 (SegFault when disabling ATTR_EMULATE_PREPARES and
    MySQL 8.0). (Nikita)

- MySQLi:
  . Fixed bug #74779 (x() and y() truncating floats to integers). (cmb)

- Opcache:
  . Fixed bug #80634 (write_property handler of internal classes is skipped on
    preloaded JITted code). (Dmitry)
  . Fixed bug #80682 (opcache doesn't honour pcre.jit option). (Remi)
  . Fixed bug #80742 (Opcache JIT makes some boolean logic unexpectedly be
    true). (Dmitry)
  . Fixed bug #80745 (JIT produces Assert failure and UNKNOWN:0 var_dumps in
    code involving bitshifts). (Dmitry)

- OpenSSL:
  . Fixed bug #80747 (Providing RSA key size < 512 generates key that crash
    PHP). (Nikita)

- Phar:
  . Fixed bug #75850 (Unclear error message wrt. __halt_compiler() w/o
    semicolon) (cmb)
  . Fixed bug #70091 (Phar does not mark UTF-8 filenames in ZIP archives). (cmb)
  . Fixed bug #53467 (Phar cannot compress large archives). (cmb, lserni)

- Socket:
  . Fixed bug #80723 (Different sockets compare as equal (regression in 8.0)).
    (Nikita)

- SPL:
  . Fixed bug#80719 (Iterating after failed ArrayObject::setIteratorClass()
    causes Segmentation fault). (Nikita)

- Standard:
  . Fixed bug #80654 (file_get_contents() maxlen fails above (2**31)-1 bytes).
    (cmb)
  . Fixed bug #80718 (ext/standard/dl.c fallback code path with syntax error).
    (Nikita)

21 Jan 2021, PHP 8.0.2

- Core:
  . Fixed bug #80523 (bogus parse error on >4GB source code). (Nikita)
  . Fixed bug #80384 (filter buffers entire read until file closed). (Adam
    Seitz, cmb)
  . Fixed bug #80596 (Invalid union type TypeError in anonymous classes).
    (Daniil Gentili)
  . Fixed bug #80617 (GCC throws warning about type narrowing in
    ZEND_TYPE_INIT_CODE). (Nikita)

- BCMath:
  . Fixed bug #80545 (bcadd('a', 'a') doesn't throw an exception).
    (Jens de Nies)

- Curl:
  . Fixed bug #80595 (Resetting POSTFIELDS to empty array breaks request). (cmb)

- Date:
  . Fixed bug #80376 (last day of the month causes runway cpu usage). (Derick)

- DOM:
  . Fixed bug #80537 (Wrong parameter type in DOMElement::removeAttributeNode
    stub). (Nikita)

- Filter:
  . Fixed bug #80584 (0x and 0X are considered valid hex numbers by
    filter_var()). (girgias)

- GMP:
  . Fixed bug #80560 (Strings containing only a base prefix return 0 object).
    (girgias)

- Intl:
  . Fixed bug #80644 (Missing resource causes subsequent get() calls to fail).
    (Nikita)

- MySQLi:
  . Fixed bug #67983 (mysqlnd with MYSQLI_OPT_INT_AND_FLOAT_NATIVE fails to
    interpret bit columns). (Nikita)
  . Fixed bug #64638 (Fetching resultsets from stored procedure with cursor
    fails). (Nikita)
  . Fixed bug #72862 (segfault using prepared statements on stored procedures
    that use a cursor). (Nikita)
  . Fixed bug #77935 (Crash in mysqlnd_fetch_stmt_row_cursor when calling an SP
    with a cursor). (Nikita)

- ODBC:
  . Fixed bug #80592 (all floats are the same in ODBC parameters). (cmb)

- Opcache:
  . Fixed bug #80422 (php_opcache.dll crashes when using Apache 2.4 with JIT).
    (Dmitry)

- PDO_Firebird:
  . Fixed bug #80521 (Parameters with underscores no longer recognized). (cmb,
    Simonov Denis)

- Phar:
  . Fixed bug #76929 (zip-based phar does not respect phar.require_hash).
    (david at bamsoftware, cmb)
  . Fixed bug #77565 (Incorrect locator detection in ZIP-based phars). (cmb)
  . Fixed bug #69279 (Compressed ZIP Phar extractTo() creates garbage files).
    (cmb)

- Phpdbg:
  . Reverted fix for bug #76813 (Access violation near NULL on source operand).
    (cmb)

07 Jan 2021, PHP 8.0.1

- Core:
  . Fixed bug #80345 (PHPIZE configuration has outdated PHP_RELEASE_VERSION).
    (cmb)
  . Fixed bug #72964 (White space not unfolded for CC/Bcc headers). (cmb)
  . Fixed bug #80391 (Iterable not covariant to mixed). (Nikita)
  . Fixed bug #80393 (Build of PHP extension fails due to configuration gap
    with libtool). (kir dot morozov at gmail dot com)
  . Fixed bug #77069 (stream filter loses final block of data). (cmb)

- Fileinfo:
  . Fixed bug #77961 (finfo_open crafted magic parsing SIGABRT). (cmb)

- FPM:
  . Fixed bug #69625 (FPM returns 200 status on request without
    SCRIPT_FILENAME env). (Jakub Zelenka)

- IMAP
  . Fixed bug #80438 (imap_msgno() incorrectly warns and return false on valid UIDs in PHP 8.0.0). (girgias)
  . Fix a regression with valid UIDs in imap_savebody() (girgias)
  . Make warnings for invalid message numbers/UIDs between functions consistent (girgias)

- Intl:
  . Fixed bug #80425 (MessageFormatAdapter::getArgTypeList redefined). (Nikita)

- OCI8
  . Create Windows DLLs for Oracle Client 19c. (cmb)

- Opcache:
  . Fixed bug #80404 (Incorrect range inference result when division results
    in float). (Nikita)
  . Fixed bug #80377 (Opcache misses executor_globals). (Nikita)
  . Fixed bug #80433 (Unable to disable the use of the AVX command when using
    JIT). (Nikita)
  . Fixed bug #80447 (Strange out of memory error when running with JIT).
    (Dmitry)
  . Fixed bug #80480 (Segmentation fault with JIT enabled). (Dmitry)
  . Fixed bug #80506 (Immediate SIGSEGV upon ini_set("opcache.jit_debug", 1)).
    (Dmitry)

- OpenSSL:
  . Fixed bug #80368 (OpenSSL extension fails to build against LibreSSL due to
    lack of OCB support). (Nikita)

- PDO MySQL:
  . Fixed bug #80458 (PDOStatement::fetchAll() throws for upsert queries).
    (Kamil Tekiela)
  . Fixed bug #63185 (nextRowset() ignores MySQL errors with native prepared
    statements). (Nikita)
  . Fixed bug #78152 (PDO::exec() - Bad error handling with multiple commands).
    (Nikita)
  . Fixed bug #66878 (Multiple rowsets not returned unless PDO statement object
    is unset()). (Nikita)
  . Fixed bug #70066 (Unexpected "Cannot execute queries while other unbuffered
    queries"). (Nikita)
  . Fixed bug #71145 (Multiple statements in init command triggers unbuffered
    query error). (Nikita)
  . Fixed bug #76815 (PDOStatement cannot be GCed/closeCursor-ed when a
    PROCEDURE resultset SIGNAL). (Nikita)
  . Fixed bug #79872 (Can't execute query with pending result sets). (Nikita)
  . Fixed bug #79131 (PDO does not throw an exception when parameter values are
    missing). (Nikita)
  . Fixed bug #72368 (PdoStatement->execute() fails but does not throw an
    exception). (Nikita)
  . Fixed bug #62889 (LOAD DATA INFILE broken). (Nikita)
  . Fixed bug #67004 (Executing PDOStatement::fetch() more than once prevents
    releasing resultset). (Nikita)
  . Fixed bug #79132 (PDO re-uses parameter values from earlier calls to
    execute()). (Nikita)

- Phar:
  . Fixed bug #73809 (Phar Zip parse crash - mmap fail). (cmb)
  . Fixed bug #75102 (`PharData` says invalid checksum for valid tar). (cmb)
  . Fixed bug #77322 (PharData::addEmptyDir('/') Possible integer overflow).
    (cmb)

- Phpdbg:
  . Fixed bug #76813 (Access violation near NULL on source operand). (cmb)

- SPL:
  . Fixed #62004 (SplFileObject: fgets after seek returns wrong line). (cmb)

- Standard:
  . Fixed bug #77423 (FILTER_VALIDATE_URL accepts URLs with invalid userinfo).
    (CVE-2020-7071) (cmb)
  . Fixed bug #80366 (Return Value of zend_fstat() not Checked). (sagpant, cmb)

- Tidy:
  . Fixed bug #77594 (ob_tidyhandler is never reset). (cmb)

- Tokenizer:
  . Fixed bug #80462 (Nullsafe operator tokenize with TOKEN_PARSE flag fails).
    (Nikita)

- XML:
  . XmlParser opaque object renamed to XMLParser for consistency with other XML objects. (girgias)

- Zlib:
  . Fixed #48725 (Support for flushing in zlib stream). (cmb)

26 Nov 2020, PHP 8.0.0

- BZ2:
  . Fixed bug #71263 (fread() does not report bzip2.decompress errors). (cmb)

- CLI:
  . Allow debug server binding to an ephemeral port via `-S localhost:0`. (Sara)

- COM:
  . Fixed bug #55847 (DOTNET .NET 4.0 GAC new location). (cmb)
  . Fixed bug #62474 (com_event_sink crashes on certain arguments). (cmb)

- Calendar:
  . Fixed bug #80007 (Potential type confusion in unixtojd() parameter parsing).
    (Andy Postnikov)

- Core:
  . Fixed bug #36365 (scandir duplicates file name at every 65535th file).
    (cmb)
  . Fixed bug #49555 (Fatal error "Function must be a string" message should be
    renamed). (Nikita)
  . Fixed bug #62294 (register_shutdown_function() does not correctly handle
    exit code). (Nikita)
  . Fixed bug #62609 (Allow implementing Traversable on abstract classes).
    (Nikita)
  . Fixed bug #65274 (Enhance undefined class constant error with class name).
    (Nikita)
  . Fixed bug #65275 (Calling exit() in a shutdown function does not change the
    exit value in CLI). (Nikita)
  . Fixed bug #69084 (Unclear error message when not implementing a renamed
    abstract trait function). (Nikita)
  . Fixed bug #70839 (Converting optional argument to variadic forbidden by LSP
    checks). (Nikita)
  . Fixed bug #74558 (Can't rebind closure returned by Closure::fromCallable()).
    (cmb)
  . Fixed bug #77561 (Shebang line not stripped for non-primary script).
    (Nikita)
  . Fixed bug #77619 (Wrong reflection on MultipleIterator::__construct).
    (Fabien Villepinte)
  . Fixed bug #77966 (Cannot alias a method named "namespace"). (Nikita)
  . Fixed bug #78236 (convert error on receiving variables when duplicate [).
    (cmb)
  . Fixed bug #78770 (Incorrect callability check inside internal methods).
    (Nikita)
  . Fixed bug #79108 (Referencing argument in a function makes it a reference
    in the stack trace). (Nikita)
  . Fixed bug #79368 ("Unexpected end of file" is not an acceptable error
    message). (Alex Dowad)
  . Fixed bug #79462 (method_exists and property_exists incoherent behavior).
    (cmb)
  . Fixed bug #79467 (data:// wrappers are writable). (cmb)
  . Fixed bug #79521 (Check __set_state structure). (carusogabriel)
  . Fixed bug #79790 ("Illegal offset type" exception during AST evaluation
    not handled properly). (Nikita)
  . Fixed bug #79791 (Assertion failure when unsetting variable during binary
    op). (Nikita)
  . Fixed bug #79828 (Segfault when trying to access non-existing variable).
    (Nikita)
  . Fixed bug #79841 (Syntax error in configure / unescaped "[]" in php.m4).
    (Nikita)
  . Fixed bug #79852 (count(DOMNodeList) doesn't match
    count(IteratorIterator(DOMNodeList))). (Nikita)
  . Fixed bug #79867 (Promoted untyped properties should get null default
    value). (Nikita)
  . Fixed bug #79897 (Promoted constructor params with attribs cause crash).
    (Deus Kane)
  . Fixed bug #79927 (Generator doesn't throw exception after multiple yield
    from iterable). (Nikita)
  . Fixed bug #79946 (Build fails due to undeclared UINT32_C). (Nikita)
  . Fixed bug #79948 (Exit in auto-prepended file does not abort PHP execution).
    (Nikita)
  . Fixed bug #80045 (memleak after two set_exception_handler calls with
    __call). (Nikita)
  . Fixed bug #80096 (Segmentation fault with named arguments in nested call).
    (Nikita)
  . Fixed bug #80109 (Cannot skip arguments when extended debug is enabled).
    (Nikita)
  . Fixed bug #80225 (broken namespace usage in eval code). (Nikita)
  . Fixed bug #80258 (Windows Deduplication Enabled, randon permission errors).
    (cmb)
  . Fixed bug #80280 (ADD_EXTENSION_DEP() fails for ext/standard and ext/date).
    (cmb)
  . Fixed bug #80334 (assert() vs named parameters - confusing error). (Nikita)
  . Fixed bug #80055 (Abstract trait methods returning "self" cannot be
    fulfilled by traits). (Nikita)
  . Fixed faulty generator cleanup with yield from. (Bob)
  . Implement #[Attr] Attribute syntax as per final vote in RFC
    https://wiki.php.net/rfc/shorter_attribute_syntax_change
  . Implemented FR #47074 (phpinfo() reports "On" as 1 for the some
    extensions). (cmb)
  . Implemented FR #72089 (require() throws fatal error instead of exception).
    (Nikita)
  . Removed the pdo_odbc.db2_instance_name php.ini directive. (Kalle)
  . Use SSE2 instructions do locale independent strtolower. (Laruence)

- Curl:
  . Bumped required libcurl version to 7.29.0. (cmb)
  . Fixed bug #80121 (Null pointer deref if CurlHandle directly instantiated).
    (Nikita)

- DOM:
  . Add property DOMXPath::$registerNodeNamespaces and constructor argument
    that allow global flag to configure query() or evaluate() calls.
  . Fixed bug #79968 (DOMChildNode API crash on unattached nodes). (Benjamin)
  . Fixed bug #80268 (loadHTML() truncates at NUL bytes). (cmb)

- Date:
  . Fixed bug #60302 (DateTime::createFromFormat should new static(), not new
    self()). (Derick)
  . Fixed bug #65547 (Default value for sunrise/sunset zenith still wrong).
    (cmb)
  . Fixed bug #69044 (discrepancy between time and microtime). (krakjoe)
  . Fixed bug #80057 (DateTimeImmutable::createFromFormat() does not populate
    time). (Derick)
  . Implemented FR #79903 (datetime: new format "p", same as "P" but returning
    "Z" for UTC). (gharlan)

- Enchant:
  . Add LIBENCHANT_VERSION macro.
  . Add enchant_dict_add and enchant_dict_is_added functions.
  . Deprecate enchant_broker_set_dict_path, enchant_broker_get_dict_path,
    enchant_dict_add_to_personal and enchant_dict_is_in_session.
  . Use libenchant-2 when available.

- FFI:
  . Added FFI\CType::getName() method. (chopins)
  . Fixed bug #79177 (FFI doesn't handle well PHP exceptions within callback).
    (cmb, Dmitry, Nikita)
  . Fixed bug #79749 (Converting FFI instances to bool fails). (cmb)

- FPM:
  . Add pm.status_listen option. (Jakub Zelenka)

- Fileinfo:
  . Upgrade to libmagic 5.39. (Anatol)

- GD:
  . Added imagegetinterpolation(). (cmb)
  . Fixed bug #55005 (imagepolygon num_points requirement). (cmb)
  . Made the $num_points parameter of php_imagepolygon optional. (cmb)
  . Removed deprecated image2wbmp(). (cmb)
  . Removed deprecated png2wbmp() and jpeg2wbmp(). (cmb)
  . Replaced gd resources with objects. (Mark Randall)

- IMAP:
  . Fixed bug #64076 (imap_sort() does not return FALSE on failure). (cmb)
  . Fixed bug #76618 (segfault on imap_reopen). (girgias)
  . Fixed bug #80213 (imap_mail_compose() segfaults on certain $bodies). (cmb)
  . Fixed bug #80215 (imap_mail_compose() may modify by-val parameters). (cmb)
  . Fixed bug #80216 (imap_mail_compose() does not validate types/encodings).
    (cmb)
  . Fixed bug #80220 (imap_mail_compose() may leak memory). (cmb)
  . Fixed bug #80223 (imap_mail_compose() leaks envelope on malformed bodies).
    (cmb)
  . Fixed bug #80226 (imap_sort() leaks sortpgm memory). (cmb)
  . Fixed bug #80239 (imap_rfc822_write_address() leaks memory). (cmb)
  . Fixed bug #80242 (imap_mail_compose() segfaults for multipart with rfc822).
    (cmb)
  . Fixed minor regression caused by fixing bug #80220. (cmb)

- Iconv:
  . Dropped support for iconv without proper errno setting. (cmb)

- Intl:
  . Removed deprecated INTL_IDNA_VARIANT_2003. (cmb)

- JIT:
  . Fixed bug #77857 (Wrong result if executed with JIT). (Laruence)
  . Fixed bug #79255 (PHP cannot be compiled with enable JIT).
    (Laruence, Dmitry)
  . Fixed bug #79582 (Crash seen when opcache.jit=1235 and
    opcache.jit_debug=2). (Laruence)
  . Fixed bug #79743 (Fatal error when assigning to array property
    with JIT enabled). (Laruence)
  . Fixed bug #79864 (JIT segfault in Symfony OptionsResolver). (Dmitry)
  . Fixed bug #79888 (Incorrect execution with JIT enabled). (Dmitry)

- JSON:
  . The JSON extension is now an integral part of PHP and cannot be disabled
    as per RFC: https://wiki.php.net/rfc/always_enable_json (tandre)

- LDAP:
  . Fixed memory leaks. (ptomulik)
  . Removed deprecated ldap_sort. (mcmic)

- MBString:
  . Fixed bug #76999 (mb_regex_set_options() return current options). (cmb)
  . Removed the unused $is_hex parameter from mb_decode_numericentity(). (cmb)

- MySQLi:
  . Fixed bug #76809 (SSL settings aren't respected when persistent connections
    are used). (fabiomsouto)

- Mysqlnd:
  . Fixed #60594 (mysqlnd exposes 160 lines of stats in phpinfo). (PeeHaa)

- OCI8:
  . Deprecated old OCI8 function aliases. (Jens de Nies)
  . Modernized oci_register_taf_callback() callable argument parsing
    implementation. (girgias)
  . Removed obsolete no-op function oci_internal_debug(). (Jens de Nies)

- ODBC:
  . Fixed bug #22986 (odbc_connect() may reuse persistent connection). (cmb)
  . Fixed bug #44618 (Fetching may rely on uninitialized data). (cmb)

- Opcache:
  . Fixed bug #76535 (Opcache does not replay compile-time warnings). (Nikita)
  . Fixed bug #78654 (Incorrectly computed opcache checksum on files with
    non-ascii characters). (mhagstrand)
  . Fixed bug #79665 (ini_get() and opcache_get_configuration() inconsistency).
    (cmb)
  . Fixed bug #80030 (Optimizer segfault with isset on static property with
    undef dynamic class name). (Nikita)
  . Fixed bug #80175 (PHP8 RC1 - JIT Buffer not working). (cmb)
  . Fixed bug #80184 (Complex expression in while / if statements resolves to
    false incorrectly). (Nikita)
  . Fixed bug #80255 (Opcache bug (bad condition result) in 8.0.0rc1). (Nikita)
  . Fixed run-time binding of preloaded dynamically declared function. (Dmitry)

- OpenSSL:
  . Added Cryptographic Message Syntax (CMS) support. (Eliot Lear)

- PCRE:
  . Don't ignore invalid escape sequences. (sjon)
  . Updated to PCRE2 10.35. (cmb)

- PDO:
  . Changed default PDO error mode to exceptions. (AllenJB)
  . Fixed bug #77849 (Disable cloning of PDO handle/connection objects).
    (camporter)

- PDO_Firebird:
  . Fixed bug #64937 (Firebird PDO preprocessing sql). (Simonov Denis)

- PDO_OCI:
  . Added support for setting and getting the oracle OCI 18c call timeout.
    (camporter)

- PDO_PGSQL:
  . Bumped required libpq version to 9.1. (cmb)

- PGSQL:
  . Bumped required libpq version to 9.1. (cmb)

- Phpdbg:
  . Fixed bug #76596 (phpdbg support for display_errors=stderr). (kabel)
  . Fixed bug #76801 (too many open files). (alekitto)
  . Fixed bug #77800 (phpdbg segfaults on listing some conditional breakpoints).
    (krakjoe)
  . Fixed bug #77805 (phpdbg build fails when readline is shared). (krakjoe)

- Reflection:
  . Fixed bug #64592 (ReflectionClass::getMethods() returns methods out of
    scope). (Nikita)
  . Fixed bug #69180 (Reflection does not honor trait conflict resolution /
    method aliasing). (Nikita)
  . Fixed bug #74939 (Nested traits' aliased methods are lowercased). (Nikita)
  . Fixed bug #77325 (ReflectionClassConstant::$class returns wrong class when
    extending). (Nikita)
  . Fixed bug #78697 (ReflectionClass::implementsInterface - inaccurate error
    message with traits). (villfa)
  . Fixed bug #80190 (ReflectionMethod::getReturnType() does not handle static
    as part of union type). (Nikita)
  . Fixed bug #80299 (ReflectionFunction->invokeArgs confused in arguments).
    (Nikita)
  . Fixed bug #80370 (getAttributes segfault on dynamic properties). (Benjamin
    Eberlei)
  . Implement #79628 (Add $filter parameter for ReflectionClass::getConstants
    and ReflectionClass::getReflectionConstants) (carusogabriel)
  . Implement ReflectionProperty::hasDefaultValue and
    Reflection::getDefaultValue (beberlei)

- SNMP:
  . Fixed bug #70461 (disable md5 code when it is not supported in net-snmp).
    (Alexander Bergmann, cmb)

- SPL:
  . Fixed bug #65006 (spl_autoload_register fails with multiple callables using
    self, same method). (Nikita)
  . Fixed bug #65387 (Circular references in SPL iterators are not garbage
    collected). (Nikita)
  . Fixed bug #71236 (Second call of spl_autoload_register() does nothing if it
    has no arguments). (Nikita)
  . Fixed bug #79987 (Memory leak in SplFileInfo because of missing
    zend_restore_error_handling()). (Dmitry)
  . SplFixedArray is now IteratorAggregate rather than Iterator. (alexdowad)

- SQLite3:
  . Added SQLite3::setAuthorizer() and respective class constants. (bohwaz)

- Session:
  . Fixed bug #73529 (session_decode() silently fails on wrong input). (cmb)
  . Fixed bug #78624 (session_gc return value for user defined session
    handlers). (bshaffer)

- Shmop:
  . Converted shmop resources to objects. (cmb)

- SimpleXML:
  . Fixed bug #63575 (Root elements are not properly cloned). (cmb)
  . Fixed bug #75245 (Don't set content of elements with only whitespaces).
    (eriklundin)

- Sodium:
  . Fixed bug #77646 (sign_detached() strings not terminated). (Frank)

- Standard:
  . Don't force rebuild of symbol table, when populating $http_response_header
    variable by the HTTP stream wrapper. (Dmitry)
  . Fixed bug #47983 (mixed LF and CRLF line endings in mail()). (cmb)
  . Fixed bug #64060 (lstat_stat_variation7.phpt fails on certain file systems).
    (M. Voelker, cmb)
  . Fixed bug #75902 (str_replace should warn when misused with nested arrays).
    (Nikita)
  . Fixed bug #76859 (stream_get_line skips data if used with data-generating
    filter). (kkopachev)
  . Fixed bug #77204 (getimagesize(): Read error! should mention file path).
    (peter279k)
  . Fixed bug #78385 (parse_url() does not include 'query' when question mark
    is the last char). (Islam Israfilov)
  . Fixed bug #79868 (Sorting with array_unique gives unwanted result). (Nikita)
  . Fixed bug #80256 (file_get_contents strip first line with chunked encoding
    redirect). (Nikita)
  . Fixed bug #80266 (parse_url silently drops port number 0). (cmb, Nikita)
  . Fixed bug #80290 (Double free when ASSERT_CALLBACK is used with a dynamic
    message). (Nikita)
  . Implemented FR #78638 (__PHP_Incomplete_Class should be final). (Laruence)
  . Made quoting of cmd execution functions consistent. (cmb)

- Tidy:
  . Removed the unused $use_include_path parameter from tidy_repair_string().
    (cmb)

- Tokenizer:
  . Fixed bug #80328 (PhpToken::getAll() confusing name). (Nikita)

- XML:
  . Fixed bug #76874 (xml_parser_free() should never leak memory). (Nikita)

- XMLWriter:
  . Changed functions to accept/return XMLWriter objects instead of resources.
    (cmb)
  . Implemented FR #79344 (xmlwriter_write_attribute_ns: $prefix should be
    nullable). (cmb)
  . Removed return types from XMLWriter stubs. (cmb)

- Zip:
  . Add "flags" options to ZipArchive::addGlob and addPattern methods
    keeping previous behavior having FL_OVERWRITE by default. (Remi)
  . Add ZipArchive::EM_UNKNOWN and ZipArchive::EM_TRAD_PKWARE constants. (Remi)
  . Add ZipArchive::isCompressionMethodSupported() and
    ZipArchive::isEncryptionMethodSupported() method (libzip 1.7.0). (Remi)
  . Add ZipArchive::replaceFile() method. (Remi)
  . Add ZipArchive::setCancelCallback method (since libzip 1.6.0). (Remi)
  . Add ZipArchive::setMtimeName and ZipArchive::setMtimeIndex methods. (Remi)
  . Add ZipArchive::setProgressCallback method (since libzip 1.3.0). (Remi)
  . Add lastId property to ZipArchive. (Remi)
  . Add optional "flags" parameter to ZipArchive::addEmptyDir, addFile and
    addFromString methods. (Remi)
  . Fixed bug #50678 (files extracted by ZipArchive class lost their
    original modified time). (Remi)
  . Fixed bug #72374 (remove_path strips first char of filename). (tyage, Remi)
  . Implemented FR #77960 (add compression / encryption options for
    ZipArchive::addGlob and ZipArchive::addPattern). (Remi)
  . ZipArchive::status and ZipArchive::statusSys properties and
    ZipArchive::getStatusString() method stay valid after the archive
    is closed. (Remi)

- Zlib:
  . Fixed bug #71417 (fread() does not report zlib.inflate errors). (cmb)
  . Fixed bug #78792 (zlib.output_compression disabled by Content-Type: image/).
    (cmb)

