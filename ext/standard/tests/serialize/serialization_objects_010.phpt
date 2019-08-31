--TEST--
Serialize() must return a string or NULL
--FILE--
<?php
/* Prototype  : proto string serialize(mixed variable)
 * Description: Returns a string representation of variable (which can later be unserialized)
 * Source code: ext/standard/var.c
 * Alias to functions:
 */
/* Prototype  : proto mixed unserialize(string variable_representation)
 * Description: Takes a string representation of variable and recreates it
 * Source code: ext/standard/var.c
 * Alias to functions:
 */

Class C implements Serializable {
	public function serialize() {
		return $this;
	}

	public function unserialize($blah) {
	}
}

try {
	var_dump(serialize(new C));
} catch (Exception $e) {
	echo $e->getMessage(). "\n";
}

echo "Done";
?>
--EXPECT--
C::serialize() must return a string or NULL
Done
