--TEST--
Phar - test specific manifest length
--INI--
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php

$files = array(
    "lib/widgets/Widgets.php",
    "lib/events/FormBeginEventArgs.php",
    "lib/events/FormEndEventArgs.php",
    "lib/Core.php",
    "lib/database/MySqlDatabase.php",
    "lib/utils/DateUtil.php",
    "js/global.js",
    "files/_emptyDirectory",
    "files/search/schema.xml",
    "vendor/Fusonic/Linq/Internal/WhereIterator.php",
    "vendor/Fusonic/Linq/Internal/SelectManyIterator.php",
    "vendor/Fusonic/Linq/Internal/SelectIterator.php",
    "vendor/Fusonic/Linq/Internal/DiffIterator.php",
    "vendor/Fusonic/Linq/Internal/GroupIterator.php",
    "vendor/Fusonic/Linq/Internal/DistinctIterator.php",
    "vendor/Fusonic/Linq/Internal/LinqHelper.php",
    "vendor/Fusonic/Linq/Internal/OrderIterator.php",
    "vendor/Fusonic/Linq/Internal/IntersectIterator.php",
    "vendor/Fusonic/Linq/GroupedLinq.php",
    "vendor/Fusonic/Linq.php",
    "vendor/Fusonic/UI/Widgets/Forms/FormBegin.php",
    "vendor/Fusonic/UI/Widgets/Forms/FormSectionBuilder.php",
    "vendor/Fusonic/UI/Widgets/Forms/AutoSelect.php",
    "vendor/Fusonic/UI/Widgets/Forms/ControlGroup.php",
    "vendor/Fusonic/UI/Widgets/Forms/FormEnd.php",
    "vendor/Fusonic/UI/Widgets/WidgetBase.php",
    "modules/calendar/ajax/calendarGetInvitedUsersContentAjaxHandler.php",
    "modules/calendar/js/calendarAppointmentForm.js",
    "modules/calendar/misc/calendarAppointment.php",
    "modules/calendar/pages/forms/calendarAppointmentForm.php",
    "modules/calendar/setup/config.xml",
    "modules/cmt/js/cmtMicroCommentsWidget.js",
    "modules/cmt/setup/config.xml",
    "modules/meta/misc/metaContentHelper.php",
    "modules/meta/setup/config.xml",
    "modules/brd/misc/brdPostStreamFormatter.php",
    "modules/brd/misc/brdPost.php",
    "modules/brd/setup/config/streamContents.xml",
    "modules/brd/setup/resources/lang/en.xml",
    "modules/brd/setup/resources/lang/de.xml",
    "modules/brd/setup/config.xml",
    "modules/auth/misc/authLoginService.php",
    "modules/auth/setup/config.xml",
    "modules/bwd/cache/bwdWordCacheCreator.php",
    "modules/bwd/bwd.php",
    "modules/bwd/setup/config.xml",
    "modules/nws/templates/pages/forms/nwsNewsForm.tpl",
    "modules/nws/templates/pages/nwsShowNewsPage.tpl",
    "modules/nws/pages/forms/nwsNewsForm.php",
    "modules/nws/pages/nwsShowNewsPage.php",
    "modules/nws/setup/config.xml",
    "modules/gmp/cache/gmpMarkersCacheCreator.php",
    "modules/gmp/select/gmpMapContentSelect.php",
    "modules/gmp/templates/gmpShowAppointmentPage.tpl",
    "modules/gmp/templates/gmpShowLinkPage.tpl",
    "modules/gmp/setup/config.xml",
    "modules/mul/cache/mulVideoPortalCacheCreator.php",
    "modules/mul/misc/mulPermissionHelper.php",
    "modules/mul/templates/widgets/mulFileEmbedWidget_Video_Flv.tpl",
    "modules/mul/setup/config/mulUploadVideoPortalMatches.xml",
    "modules/mul/setup/config.xml",
    "modules/cat/select/catCategorySelect.php",
    "modules/cat/misc/catCategory.php",
    "modules/cat/templates/pages/forms/catCategoryForm.tpl",
    "modules/cat/pages/forms/catEditCategoryForm.php",
    "modules/cat/pages/forms/catAddCategoryForm.php",
    "modules/cat/setup/config.xml",
    "modules/sty/events/styPageShowEventHandler.php",
    "modules/sty/misc/styBox.php",
    "modules/sty/templates/pages/forms/styLayoutForm.tpl",
    "modules/sty/templates/pages/forms/styBoxForm.tpl",
    "modules/sty/templates/pages/forms/styVariantForm.tpl",
    "modules/sty/setup/resources/lang/en.xml",
    "modules/sty/setup/resources/lang/de.xml",
    "modules/sty/setup/config.xml",
    "modules/reg/misc/regRegistrationHelper.php",
    "modules/reg/setup/config.xml",
    "modules/not/misc/notEmailNotificationProvider.php",
    "modules/not/setup/config.xml",
    "modules/styfusonic/setup/config.xml",
    "modules/sys/ajax/sysUserAutoSuggestSelectAjaxHandler.php",
    "modules/sys/js/sysUserAutoSuggestSelect.js",
    "modules/sys/select/sysPermissionSelect.php",
    "modules/sys/misc/sysHtaccessConfigWriter.php",
    "modules/sys/misc/sysUserRepository.php",
    "modules/sys/setup/resources/lang/en.xml",
    "modules/sys/setup/resources/lang/de.xml",
    "modules/sys/setup/config.xml",
    "modules/igr/boxes/igrGreatestEntriesBoxTab.php",
    "modules/igr/boxes/igrTopRatedEntriesBoxTab.php",
    "modules/igr/setup/config.xml",
    "modules/rat/ajax/ratRateAjaxHandler.php",
    "modules/rat/ajax/ratUnlikeAjaxHandler.php",
    "modules/rat/setup/config.xml",
    "modules/search/select/searchModuleSelect.php",
    "modules/search/select/searchOrderSelect.php",
    "modules/search/misc/searchResultFormatter.php",
    "modules/search/misc/searchProviderSolr.php",
    "modules/search/misc/searchProviderLucene.php",
    "modules/search/misc/searchResultItem.php",
    "modules/search/misc/searchProviderBase.php",
    "modules/search/misc/searchIProvider.php",
    "modules/search/templates/misc/searchResultFormatter.tpl",
    "modules/search/templates/pages/searchIndexPage.tpl",
    "modules/search/templates/pages/forms/searchSearchForm.tpl",
    "modules/search/pages/forms/searchSearchForm.php",
    "modules/search/css/searchResultFormatter.css",
    "modules/search/setup/config/sysSettings.xml",
    "modules/search/setup/resources/lang/en.xml",
    "modules/search/setup/resources/lang/de.xml",
    "modules/search/setup/config.xml",
    "style/Fusonic/40components.css",
    "style/_emptyDirectory",
    "index.php",
//	"a", // This will make the test pass
);

// Create Phar with the filenames above
$phar = new Phar(__DIR__ . "/bug65028.phar");
foreach($files as $file)
{
    $phar->addFromString($file, "");
}

// Copy phar
copy(__DIR__ . "/bug65028.phar", __DIR__ . "/bug65028-copy.phar");

// Open phar
try
{
    $phar = new Phar(__DIR__ . "/bug65028-copy.phar");
    echo "No exception thrown.\n";
}
catch(UnexpectedValueException $ex)
{
    echo "Exception thrown: " . $ex->getMessage() . "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug65028.phar");
@unlink(__DIR__ . "/bug65028-copy.phar");
?>
--EXPECT--
No exception thrown.
