--TEST--
Bug #68549: setting timestamp around a DST transition
--CREDITS--
Roel Harbers <roel.harbers@on2it.net>
--FILE--
<?php
$timezonesWithTimestamps = [
    'America/Los_Angeles' => [
        // LANG=en_US TZ="America/Los_Angeles" date --date=@<...>
        // forward:
        1615708800, // Sun Mar 14 00:00:00 PST 2021
        1615712399, // Sun Mar 14 00:59:59 PST 2021
        1615712400, // Sun Mar 14 01:00:00 PST 2021
        1615715999, // Sun Mar 14 01:59:59 PST 2021
        1615716000, // Sun Mar 14 03:00:00 PDT 2021
        1615716001, // Sun Mar 14 03:00:01 PDT 2021
        1615719600, // Sun Mar 14 04:00:00 PDT 2021
        // backward:
        1636271999, // Sun Nov  7 00:59:59 PDT 2021
        1636272000, // Sun Nov  7 01:00:00 PDT 2021
        1636275599, // Sun Nov  7 01:59:59 PDT 2021
        1636275600, // Sun Nov  7 01:00:00 PST 2021
        1636279199, // Sun Nov  7 01:59:59 PST 2021
        1636279200, // Sun Nov  7 02:00:00 PST 2021
    ],
    'Europe/Amsterdam' => [
        // LANG=en_US TZ="Europe/Amsterdam" date --date=@<...>
        // forward:
        1616886000, // Sun Mar 28 00:00:00 CET 2021
        1616889599, // Sun Mar 28 00:59:59 CET 2021
        1616889600, // Sun Mar 28 01:00:00 CET 2021
        1616893199, // Sun Mar 28 01:59:59 CET 2021
        1616893200, // Sun Mar 28 03:00:00 CEST 2021
        1616893201, // Sun Mar 28 03:00:01 CEST 2021
        1616896800, // Sun Mar 28 04:00:00 CEST 2021
        // backward:
        1635638399, // Sun Oct 31 01:59:59 CEST 2021
        1635638400, // Sun Oct 31 02:00:00 CEST 2021
        1635641999, // Sun Oct 31 02:59:59 CEST 2021
        1635642000, // Sun Oct 31 02:00:00 CET 2021
        1635645599, // Sun Oct 31 02:59:59 CET 2021
        1635645600, // Sun Oct 31 03:00:00 CET 2021
    ],
];
foreach ($timezonesWithTimestamps as $timezone => $timestamps) {
    foreach ($timestamps as $timestamp) {
        $tz = new DateTimeZone($timezone);
        $dt = new DateTime('now', $tz);
        $dt->setTimestamp($timestamp);
        printf("%s %d -> %d %s\n", $timezone, $timestamp, $dt->getTimestamp(), $dt->format(DateTimeInterface::ISO8601));
    }
}
?>
--EXPECT--
America/Los_Angeles 1615708800 -> 1615708800 2021-03-14T00:00:00-0800
America/Los_Angeles 1615712399 -> 1615712399 2021-03-14T00:59:59-0800
America/Los_Angeles 1615712400 -> 1615712400 2021-03-14T01:00:00-0800
America/Los_Angeles 1615715999 -> 1615715999 2021-03-14T01:59:59-0800
America/Los_Angeles 1615716000 -> 1615716000 2021-03-14T03:00:00-0700
America/Los_Angeles 1615716001 -> 1615716001 2021-03-14T03:00:01-0700
America/Los_Angeles 1615719600 -> 1615719600 2021-03-14T04:00:00-0700
America/Los_Angeles 1636271999 -> 1636271999 2021-11-07T00:59:59-0700
America/Los_Angeles 1636272000 -> 1636272000 2021-11-07T01:00:00-0700
America/Los_Angeles 1636275599 -> 1636275599 2021-11-07T01:59:59-0700
America/Los_Angeles 1636275600 -> 1636275600 2021-11-07T01:00:00-0800
America/Los_Angeles 1636279199 -> 1636279199 2021-11-07T01:59:59-0800
America/Los_Angeles 1636279200 -> 1636279200 2021-11-07T02:00:00-0800
Europe/Amsterdam 1616886000 -> 1616886000 2021-03-28T00:00:00+0100
Europe/Amsterdam 1616889599 -> 1616889599 2021-03-28T00:59:59+0100
Europe/Amsterdam 1616889600 -> 1616889600 2021-03-28T01:00:00+0100
Europe/Amsterdam 1616893199 -> 1616893199 2021-03-28T01:59:59+0100
Europe/Amsterdam 1616893200 -> 1616893200 2021-03-28T03:00:00+0200
Europe/Amsterdam 1616893201 -> 1616893201 2021-03-28T03:00:01+0200
Europe/Amsterdam 1616896800 -> 1616896800 2021-03-28T04:00:00+0200
Europe/Amsterdam 1635638399 -> 1635638399 2021-10-31T01:59:59+0200
Europe/Amsterdam 1635638400 -> 1635638400 2021-10-31T02:00:00+0200
Europe/Amsterdam 1635641999 -> 1635641999 2021-10-31T02:59:59+0200
Europe/Amsterdam 1635642000 -> 1635642000 2021-10-31T02:00:00+0100
Europe/Amsterdam 1635645599 -> 1635645599 2021-10-31T02:59:59+0100
Europe/Amsterdam 1635645600 -> 1635645600 2021-10-31T03:00:00+0100
