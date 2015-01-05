--TEST--
Bug #64677 (execution operator `` stealing surrounding arguments)
--FILE--
<?PHP
class cat {
	public function show_output($prepend, $output = '') {
	}
}
$cat = new cat();
$cat->show_output('Files: ', trim(`cd .`)); // this gives invalid args to shell_exec
$cat->show_output('Files: ', `cd .`); // this causes a segmentation fault
$cat->show_output(`cd .`); // this causes a segmentation fault

function show_outputa($prepend, $output) {
	echo "Okey";
}
show_outputa('Files: ', `cd .`); // this works as expected

?>
--EXPECT--
Okey
