--TEST--
Bug #31672 (one-line comment with </script>)
--FILE--
<?php echo "Foo"; // ?>bar
<?php echo "Foo"; // </script>bar
--EXPECT--
Foobar
Foobar
