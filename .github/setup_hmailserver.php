<?php

/** If required change these values to make the test runs */
const IMAP_MAIL_DOMAIN = 'example.com';
const IMAP_MAILBOX_PASSWORD = 'p4ssw0rd';
/** Tests require 4 valid userids */
const IMAP_USERS = ["webmaster", "info", "admin", "foo"];

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
