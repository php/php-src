--TEST--
ReflectionProperty::getDefaultValue()
--FILE--
<?php


class root
{
	public    $rPub = "rPub";
	public    $xPub = "xPub";
	protected $rPro = "rPro";
	protected $xPro = "xPub";
	private   $rPri = "rPri";
	private   $xPri = "xPri";
	
	static public $stat = "rStat";
}

class derived extends root
{
	public    $dPub = "dPub";
	public    $xPub = "nPub";
	protected $dPro = "dPro";
	protected $xPro = "nPub";
	private   $dPri = "dPri";
	private   $xPri = "nPri";	
}

function show_prop($p)
{
	echo "{$p->class}::{$p->name} = " . $p->getDefaultValue() . "\n";
}

function show_class($c)
{
	echo "===$c===\n";

	$r = new ReflectionClass($c);

	foreach($r->getProperties() as $p)
	{
		show_prop($p);
	}
}

show_class("root");
show_class("derived");

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===root===
root::rPub = rPub
root::xPub = xPub
root::rPro = rPro
root::xPro = xPub
root::rPri = rPri
root::xPri = xPri
root::stat = rStat
===derived===
derived::dPub = dPub
derived::xPub = nPub
derived::dPro = dPro
derived::xPro = nPub
derived::dPri = dPri
derived::xPri = nPri
derived::rPub = rPub
derived::rPro = rPro
derived::stat = rStat
===DONE===
