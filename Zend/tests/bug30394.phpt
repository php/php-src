--TEST--
Bug #30394 (Assignment operators yield wrong result with __get/__set)
--FILE--
<?php
class Container
{
	public function __get( $what )
	{
		return $this->_p[ $what ];
	}
	
	public function __set( $what, $value )
	{
		$this->_p[ $what ] = $value;
	}
	
	private $_p = array();
}

$c = new Container();
$c->a = 1;
$c->a += 1;
print $c->a;	// --> 2

print " - ";
$c->a += max( 0, 1 );
print $c->a;	// --> 4 (!)
?>
--EXPECT--
2 - 3
