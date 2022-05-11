<?php

require __DIR__ . "/../ext/imap/tests/setup/imap_include.inc";

$hmail = new COM("hMailServer.Application");
$hmail->authenticate("Administrator", "");

$domain = $hmail->Domains->Add();
$domain->Name = IMAP_MAIL_DOMAIN;
$domain->Active = true;
$domain->Save();

$accounts = $domain->accounts();

foreach (IMAP_USERS as $user) {
    $account = $accounts->Add();
    $account->Address = "$user@" . IMAP_MAIL_DOMAIN;
    $account->Password = IMAP_MAILBOX_PASSWORD;
    $account->Active = true;
    $account->Save();
}
