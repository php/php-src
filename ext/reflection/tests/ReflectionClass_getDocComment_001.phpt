--TEST--
ReflectionClass::getDocComment()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--INI--
opcache.save_comments=1
--FILE--
<?php
/**


		My
Doc 
		* Comment 
for A

* */
class A {}

/** My DocComment for B */
class B extends A { }

class C extends B {}

/**
 * Interface doc comment
 */




interface I {}

/*.*
 * Not a doc comment
 */
class D implements I {}

/**** Not a doc comment */
class E extends C implements I {} {}

/**?** Not a doc comment */
class F extends C implements I {} {}

/**	** Doc comment for G */
final class G extends C implements I {} {}

$classes = array('A', 'B', 'C', 'D', 'E', 'F', 'G', 'I');
foreach ($classes as $class) {
	echo "\n\n---> Doc comment for class $class:\n";
	$rc = new ReflectionClass($class);	
	var_dump($rc->getDocComment());	
}


?>
--EXPECTF--


---> Doc comment for class A:
string(%d) "/**


		My
Doc 
		* Comment 
for A

* */"


---> Doc comment for class B:
string(26) "/** My DocComment for B */"


---> Doc comment for class C:
bool(false)


---> Doc comment for class D:
bool(false)


---> Doc comment for class E:
bool(false)


---> Doc comment for class F:
bool(false)


---> Doc comment for class G:
string(27) "/**	** Doc comment for G */"


---> Doc comment for class I:
string(%d) "/**
 * Interface doc comment
 */"