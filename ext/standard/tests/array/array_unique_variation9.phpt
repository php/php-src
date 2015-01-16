--TEST--
Test array_unique() function : usage variations - more than 16 elements
--CREDITS--
Matteo Beccati
--FILE--
<?php

$a = array (
  0 => 'acls_channel',
  1 => 'acls_channel',
  2 => 'ad_zone_assoc',
  3 => 'ad_zone_assoc',
  4 => 'ad_zone_assoc',
  5 => 'affiliates',
  6 => 'affiliates',
  7 => 'affiliates',
  8 => 'banners',
  9 => 'banners',
  10 => 'banners',
  11 => 'campaigns',
  12 => 'campaigns',
  13 => 'campaigns',
  14 => 'campaigns_trackers',
  15 => 'campaigns_trackers',
  16 => 'campaigns_trackers',
  17 => 'clients',
  18 => 'clients',
  19 => 'clients',
  20 => 'data_intermediate_ad',
  21 => 'data_intermediate_ad',
  22 => 'data_summary_ad_hourly',
  23 => 'data_summary_ad_hourly',
  24 => 'data_summary_zone_country_daily',
  25 => 'data_summary_zone_country_daily',
  26 => 'data_summary_zone_country_daily',
  27 => 'data_summary_zone_country_forecast',
  28 => 'data_summary_zone_country_forecast',
  29 => 'data_summary_zone_country_forecast',
  30 => 'data_summary_zone_country_monthly',
  31 => 'data_summary_zone_country_monthly',
  32 => 'data_summary_zone_country_monthly',
  33 => 'data_summary_zone_domain_page_daily',
  34 => 'data_summary_zone_domain_page_daily',
  35 => 'data_summary_zone_domain_page_daily',
  36 => 'data_summary_zone_domain_page_forecast',
  37 => 'data_summary_zone_domain_page_forecast',
  38 => 'data_summary_zone_domain_page_forecast',
  39 => 'data_summary_zone_domain_page_monthly',
  40 => 'data_summary_zone_domain_page_monthly',
  41 => 'data_summary_zone_domain_page_monthly',
  42 => 'data_summary_zone_site_keyword_daily',
  43 => 'data_summary_zone_site_keyword_daily',
  44 => 'data_summary_zone_site_keyword_daily',
  45 => 'data_summary_zone_site_keyword_forecast',
  46 => 'data_summary_zone_site_keyword_forecast',
  47 => 'data_summary_zone_site_keyword_forecast',
  48 => 'data_summary_zone_site_keyword_monthly',
  49 => 'data_summary_zone_site_keyword_monthly',
  50 => 'data_summary_zone_site_keyword_monthly',
  51 => 'data_summary_zone_source_daily',
  52 => 'data_summary_zone_source_daily',
  53 => 'data_summary_zone_source_daily',
  54 => 'data_summary_zone_source_forecast',
  55 => 'data_summary_zone_source_forecast',
  56 => 'data_summary_zone_source_forecast',
  57 => 'data_summary_zone_source_monthly',
  58 => 'data_summary_zone_source_monthly',
  59 => 'data_summary_zone_source_monthly',
  60 => 'placement_zone_assoc',
  61 => 'placement_zone_assoc',
  62 => 'placement_zone_assoc',
  63 => 'trackers',
  64 => 'trackers',
  65 => 'trackers',
  66 => 'variables',
  67 => 'variables',
  68 => 'variables',
);

print_r(array_unique($a));
?>
--EXPECTF--
Array
(
    [0] => acls_channel
    [2] => ad_zone_assoc
    [5] => affiliates
    [8] => banners
    [11] => campaigns
    [14] => campaigns_trackers
    [17] => clients
    [20] => data_intermediate_ad
    [22] => data_summary_ad_hourly
    [24] => data_summary_zone_country_daily
    [27] => data_summary_zone_country_forecast
    [30] => data_summary_zone_country_monthly
    [33] => data_summary_zone_domain_page_daily
    [36] => data_summary_zone_domain_page_forecast
    [39] => data_summary_zone_domain_page_monthly
    [42] => data_summary_zone_site_keyword_daily
    [45] => data_summary_zone_site_keyword_forecast
    [48] => data_summary_zone_site_keyword_monthly
    [51] => data_summary_zone_source_daily
    [54] => data_summary_zone_source_forecast
    [57] => data_summary_zone_source_monthly
    [60] => placement_zone_assoc
    [63] => trackers
    [66] => variables
)
