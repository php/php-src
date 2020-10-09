--TEST--
mysqli_real_escape_string()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    require('table.inc');

    if ('фу\\\\бар' !== ($tmp = mysqli_real_escape_string($link, 'фу\\бар')))
        printf("[004] Expecting фу\\\\бар, got %s\n", $tmp);

    if ('бар\"фус' !== ($tmp = mysqli_real_escape_string($link, 'бар"фус')))
        printf("[005] Expecting бар\"фус, got %s\n", $tmp);

    if ("лала\'лали" !== ($tmp = mysqli_real_escape_string($link, "лала'лали")))
        printf("[006] Expecting лала'лали, got %s\n", $tmp);

    if ("абра\\nкадабра" !== ($tmp = mysqli_real_escape_string($link, "абра\nкадабра")))
        printf("[007] Expecting абра\\nкадабра, got %s\n", $tmp);

    if ("манда\\rин" !== ($tmp = mysqli_real_escape_string($link, "манда\rин")))
        printf("[008] Expecting \\r, got %s\n", $tmp);

    if ("иху\\0аху" !== ($tmp = mysqli_real_escape_string($link, "иху" . chr(0) . "аху")))
        printf("[009] Expecting %s, got %s\n", "иху\\0аху", $tmp);

    if (($exp='абра\\\\ка\"да\\'."'".'бра\Zсим\\nсала\\rби\\0м') !==
        ($tmp = mysqli_real_escape_string($link, "абра\\ка\"да'бра\032сим\nсала\rби" . chr(0) . "м")))
    {
        printf("[010] Expecting %s, got %s\n", $exp, $tmp, var_dump($exp, $tmp));
    }

    if ('富\\\\酒吧' !== ($tmp = mysqli_real_escape_string($link, '富\\酒吧')))
        printf("[011] Expecting 富\\\\酒吧, got %s\n", $tmp);

    if ('酒吧\"小题大做' !== ($tmp = mysqli_real_escape_string($link, '酒吧"小题大做')))
        printf("[012] Expecting 酒吧\"小题大做, got %s\n", $tmp);

    if ("拉拉\'西雅图" !== ($tmp = mysqli_real_escape_string($link, "拉拉'西雅图")))
        printf("[013] Expecting 拉拉'西雅图, got %s\n", $tmp);

    if ("阿卜拉\\n轻" !== ($tmp = mysqli_real_escape_string($link, "阿卜拉\n轻")))
        printf("[014] Expecting 阿卜拉\\n轻, got %s\n", $tmp);

    if ("张明安\\r在" !== ($tmp = mysqli_real_escape_string($link, "张明安\r在")))
        printf("[015] Expecting 张明安\\r在, got %s\n", $tmp);

    if ("竺可桢\\0空调器" !== ($tmp = mysqli_real_escape_string($link, "竺可桢" . chr(0) . "空调器")))
        printf("[016] Expecting %s, got %s\n", "竺可桢\\0空调器", $tmp);

    if (($exp='阿卜拉\\\\嘉\"达丰\\'."'".'乳罩\Z辛\\n萨拉\\r毕\\0米') !==
        ($tmp = mysqli_real_escape_string($link, "阿卜拉\\嘉\"达丰'乳罩\032辛\n萨拉\r毕" . chr(0) . "米")))
    {
        printf("[017] Expecting %s, got %s\n", $exp, $tmp, var_dump($exp, $tmp));
    }

    mysqli_close($link);

    try {
        mysqli_real_escape_string($link, 'foo');
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli object is already closed
done!
