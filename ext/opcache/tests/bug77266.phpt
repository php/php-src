--TEST--
Bug #77266 (Assertion failed in dce_live_ranges)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
final class Lock
{
	private static function clearOrphanedLocks()
	{
		$lockList = [];

		$serverMonitors = array();
		$listCount = count($lockList);
		if ( is_array($lockList) && $listCount > 0 ) {
			$v = explode(':', $value);
			if (!$serverMonitors[$v[0]]['m']) {
				$serverMonitors[$v[0]]['m'] = new ServerMonitor($v[0]);
			}

		}

	}

}
?>
okey
--EXPECT--
okey
