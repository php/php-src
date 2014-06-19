Design
======

Main assumption of the model is that the problem that we are to solve is the
problem of the very long class names in PHP libraries. We would not attempt
to take autoloader's job or create packaging model - only make names
manageable.

Namespaces are defined the following way:

Zend/DB/Connection.php:
<?php
namespace Zend\DB;

class Connection {
}

function connect() {
}
?>

Namespace definition does the following:
All class and function names inside are automatically prefixed with
namespace name. Inside namespace, local name always takes precedence over
global name. Several files may be using the same namespace.
The namespace declaration statement must be the very first statement in
the file. The only exception is "declare" statement that can be used before.

Every class and function in a namespace can be referred to by the full name
- e.g. Zend\DB\Connection or Zend\DB\connect - at any time.

<?php
require 'Zend/Db/Connection.php';
$x = new Zend\DB\Connection;
Zend\DB\connect();
?>

Namespace or class name can be imported:

<?php
require 'Zend/Db/Connection.php';
use Zend\DB;
use Zend\DB\Connection as DbConnection;

$x = new Zend\DB\Connection();
$y = new DB\connection();
$z = new DbConnection();
DB\connect();
?>

The use statement only defines name aliasing. It may create name alias for
namespace or class. The simple form of statement "use A\B\C\D;" is
equivalent to "use A\B\C\D as D;". The use statement can be used at any
time in the global scope (not inside function/class) and takes effect from 
the point of definition down to the end of file. It is recommended however to
place the use statements at the beginning of the file. The use statements have
effect only on the file where they appear.

The special "empty" namespace (\ prefix) is useful as explicit global
namespace qualification. All class and function names started from \
interpreted as global.

<?php 
namespace A\B\C;

$con = \mysql_connect(...);
?>

A special constant __NAMESPACE__ contains the name of the current namespace. 
It can be used to construct fully-qualified names to pass them as callbacks.

<?php
namespace A\B\C;

function foo() {
}

set_error_handler(__NAMESPACE__ . "\foo");
?>

In global namespace __NAMESPACE__ constant has the value of empty string.

Names inside namespace are resolved according to the following rules:

1) all qualified names are translated during compilation according to
current import rules. So if we have "use A\B\C" and then "C\D\e()"
it is translated to "A\B\C\D\e()".
2) unqualified class names translated during compilation according to
current import rules. So if we have "use A\B\C" and then "new C()" it
is translated to "new A\B\C()".
3) inside namespace, calls to unqualified functions that are defined in 
current namespace (and are known at the time the call is parsed) are 
interpreted as calls to these namespace functions.
4) inside namespace, calls to unqualified functions that are not defined 
in current namespace are resolved at run-time. The call to function foo() 
inside namespace (A\B) first tries to find and call function from current 
namespace A\B\foo() and if it doesn't exist PHP tries to call internal
function foo(). Note that using foo() inside namespace you can call only 
internal PHP functions, however using \foo() you are able to call any
function from the global namespace.
5) unqualified class names are resolved at run-time. E.q. "new Exception()"
first tries to use (and autoload) class from current namespace and in case 
of failure uses internal PHP class. Note that using "new A" in namespace 
you can only create class from this namespace or internal PHP class, however
using "new \A" you are able to create any class from the global namespace.
6) Calls to qualified functions are resolved at run-time. Call to
A\B\foo() first tries to call function foo() from namespace A\B, then
it tries to find class A\B (__autoload() it if necessary) and call its
static method foo()
7) qualified class names are interpreted as class from corresponding
namespace. So "new A\B\C()" refers to class C from namespace A\B.

Examples
--------
<?php
namespace A;
foo();   // first tries to call "foo" defined in namespace "A"
         // then calls internal function "foo"
\foo(); // calls function "foo" defined in global scope
?>

<?php
namespace A;
new B();   // first tries to create object of class "B" defined in namespace "A"
           // then creates object of internal class "B"
new \B(); // creates object of class "B" defined in global scope
?>

<?php
namespace A;
new A(); // first tries to create object of class "A" from namespace "A" (A\A)
         // then creates object of internal class "A"
?>

<?php
namespace A;
B\foo();   // first tries to call function "foo" from namespace "A\B"
            // then calls method "foo" of internal class "B"
\B\foo(); // first tries to call function "foo" from namespace "B"
            // then calls method "foo" of class "B" from global scope
?>

The worst case if class name conflicts with namespace name
<?php
namespace A;
A\foo();   // first tries to call function "foo" from namespace "A\A"
            // then tries to call method "foo" of class "A" from namespace "A"
            // then tries to call function "foo" from namespace "A"
            // then calls method "foo" of internal class "A"
\A\foo(); // first tries to call function "foo" from namespace "A"
            // then calls method "foo" of class "A" from global scope
?>

TODO
====

* Support for namespace constants?

* performance problems
  - calls to internal functions in namespaces are slower, because PHP first
    looks for such function in current namespace
  - calls to static methods are slower, because PHP first tries to look
    for corresponding function in namespace

* Extend the Reflection API?
  * Add ReflectionNamespace class
    + getName()
    + getClasses()
    + getFunctions()
    + getFiles()
  * Add getNamespace() methods to ReflectionClass and ReflectionFunction

* Rename namespaces to packages?

