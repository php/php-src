--TEST--
PDO connection DSNs and passwords are sensitive
--EXTENSIONS--
pdo
--INI--
zend.exception_ignore_args=0
--FILE--
<?php
echo "Constructor:\n";
try {
    new PDO('missing:password=dsn-secret', 'username', 'password');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}

echo "Connect with named arguments:\n";
try {
    PDO::connect(dsn: 'missing:password=dsn-secret', username: 'username', password: 'password');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}

class TestPDO extends PDO {}

echo "Inherited constructor:\n";
try {
    new TestPDO('missing:password=dsn-secret', 'username', 'password');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}

echo "Inherited connect:\n";
try {
    TestPDO::connect('missing:password=dsn-secret', 'username', 'password');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
Constructor:
PDOException: could not find driver in %s:%d
Stack trace:
#0 %s(%d): PDO->__construct(Object(SensitiveParameterValue), 'username', Object(SensitiveParameterValue))
#1 {main}
Connect with named arguments:
PDOException: could not find driver in %s:%d
Stack trace:
#0 %s(%d): PDO::connect(Object(SensitiveParameterValue), 'username', Object(SensitiveParameterValue))
#1 {main}
Inherited constructor:
PDOException: could not find driver in %s:%d
Stack trace:
#0 %s(%d): PDO->__construct(Object(SensitiveParameterValue), 'username', Object(SensitiveParameterValue))
#1 {main}
Inherited connect:
PDOException: could not find driver in %s:%d
Stack trace:
#0 %s(%d): PDO::connect(Object(SensitiveParameterValue), 'username', Object(SensitiveParameterValue))
#1 {main}
