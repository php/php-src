--TEST--
Testing multiples 'default:' in switch
--FILE--
<?php 

switch (1) {
	case 2:
		print 'foo';
		break;
	case 3:
		print 'bar';
		break;
	default:
		print 1;
		break;
	default:
		print 2;
		break;
	default:
		print 3;
		break;
}

?>
--EXPECTF--
Fatal error: Switch statements must have at most one default label in %s on line %d
