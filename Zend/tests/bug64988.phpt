--TEST--
Bug #64988 (Class loading order affects E_STRICT warning)
--FILE--
<?php
abstract class Base1 {
	public function insert(array $data){
		return array_reverse($data);
	}
}

class Noisy1 extends Base1 {
	public function insert(array $data, $option1 = Null) {
		if (!empty($option1)) {
			$data['option1'] = $option1;
		}
		return parent::insert($data);
	}
}
class Smooth1 extends Noisy1 {
	public function insert(array $data) {
		return parent::insert($data, count($data));
	}
}

$o = new Smooth1();
echo "okey";
?>
--EXPECTF--
Warning: Declaration of Smooth1::insert(array $data) should be compatible with Noisy1::insert(array $data, $option1 = NULL) in %sbug64988.php on line 20
okey
