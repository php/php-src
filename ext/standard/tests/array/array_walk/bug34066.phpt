--TEST--
Bug #34066 (recursive array_walk causes segfault)
--FILE--
<?php

    $order = array(
        "DocID"			=>	"1",
        "DocDate"		=>	"19.09.06",
        "ReSubmissionDate"	=>	"",
        "DocTyp"		=>	"Stapelauftrag",
        "CustID"		=>	"00000",
        "CustomerAddress" => array(
            array(
                "Name1"		=>	'name1',
                "Name2"		=>	'name2',
                "Name3"		=>	"",
                "City"		=>	'city',
                "Street"	=>	'street',
                "Postal"	=>	'postcode',
                "IATA"		=>	"90",
                "Country"	=>	"Deutschland",
                "ShortName"	=>	'short',
                "ContactKey"	=>	"",
                "EMail"		=>	'email@example.com',
            )
        ),
        "Text1"			=>	"",
        "Text2"			=>	"",
        "Wildcard1"		=>	"",
        "Wildcard2"		=>	"",
        "Dispatch"		=>	"Paketdienst",
        "Weight"		=>	"0,0",
        "BillingCustID"		=>	"4300200000",
        "ExtDocNr"		=>	"00000000003",
        "AnalysisLock"		=>	"",
        "PrintFlag"		=>	"",
        "FormType"		=>	"0",
        "Curr"			=>	"EUR",
        "ExChangeRate"		=>	"1,0000",
        "WIRRate"		=>	"0",
        "OneTimeCustomer" => array(
            array(
                "BankCode"	=>	"",
                "BankAccount"	=>	""
            )
        ),
        "Language"		=>	"0",
        "PriceGroup"		=>	"1",
        "PrFlag"		=>	"0",
        "SalesTaxKey"		=>	"1",
        "ProceedKey"		=>	"0",
        "CustDiscountGroup"	=>	"0",
        "Discount" => array(
            array(
                "FinDisc1"	=>	"0,00",
                "Disc1Base"	=>	"145,72",
                "Disc1Value"	=>	"0,00",
                "FinDisc2"	=>	"0,00",
                "Disc2Base"	=>	"145,72",
                "Disc2Value"	=>	"0,00",
                "FinDisc3"	=>	"0,00",
                "Disc3Base"	=>	"145,72",
                "Disc3Value"	=>	"0,00",
                "ValueSummary"	=>	"0,00"
            )
        ),
        "Contact" => array(
            array(
                "Repr"		=>	"999",
                "Region"	=>	"99",
                "Commission"	=>	"0,00",
                "Agent"		=>	"000000"
            )
        ),
        "Booking" => array(
            array(
                "CostUnit"	=>	"0000000000",
                "CostCentre"	=>	"0000000000",
                "AccountingArea"=>	"01"
            )
        ),
        "InvoiceCycleKey"	=>	"0",
        "AnalysisKey"		=>	"",
        "OrderNumber"		=>	"",
        "OrderDate"		=>	"",
        "OrderCode"		=>	"",
        "DocItems" => array(
            "DocItem" => array(
                "PosType"		=>	"1",
                "ItemRef"		=>	"1002",
                "CRef"			=>	"",
                "Desc1"			=>	"Pr�sentation Niederlande per",
                "Desc2"			=>	"",
                "ArticleGroup"		=>	"102",
                "PosTypeVersion"	=>	"E",
                "Delivery" => array(
                    array(
                        "DelWeek"	=>	"",
                        "DelDay"	=>	"",
                        "DelTime"	=>	""
                    )
                ),
        "PricePu"		=>	"145,72",
        "PriceUnit"		=>	"0",
        "PriceCalculation"	=>	"0",
        "ItemVal"		=>	"145,72",
        "InputKey"		=>	"0",
        "AveragePurchasePrice"	=>	"0",
                "Tax" => array(
                    array(
                        "TaxCode"	=>	"00",
                        "TaxBra"	=>	"000",
                        "TaxBraAccess"	=>	"0",
                        "TaxSumIndex"	=>	"0"
                    )
                ),
                "DiscountArticle" => array(
                    array(
                        "DiscPC"	=>	"0,00",
                        "DiscKey"	=>	"1"
                    )
                ),
                "ProceedKeyArticle"	=>	"01",
                "ActionKey"		=>	"00",
                "ContactCommissionArticle"=>	"0,00",
                "QuantdependentPriceKey"=>	"",
                "Quant"			=>	"1",
                "QuantUnit"		=>	"",
                "Meas" => array(
                    array(
                        "Count"		=>	"1",
                        "Length"	=>	"0,000",
                        "Width"		=>	"0,000",
                        "Height"	=>	"0,000"
                    )
                ),
                "DecimalPlace"		=>	"0",
                "MultiplierQuant"	=>	"1,000000",
                "DifferingQuantUnit"	=>	"",
                "DecimalPlaceConversion"=>	"0",
                "WeightArticle" => array(
                    array(
                        "Amount"	=>	"0",
                        "Unit"		=>	"0"
                    )
                ),
                "Wreath"		=>	"0,000",
                "Stock"			=>	"1",
                "CostUnitArticle"	=>	"",
                "SerialNbKey"		=>	"0",
                "TextComplementKey"	=>	"0",
                "PartsListPrintKey"	=>	"",
                "Prod"			=>	"0000000000"
            )
        ),
        "Payment" => array(
            "PaymentKey"		=>	"0",
            "ReminderKey"		=>	"00",
            "PayTerms" => array(
                array(
                    "PayTerm"		=>	"1",
                    "PayDays"		=>	"000",
                    "CashDiscDays1"	=>	"000",
                    "CashDiscDays2"	=>	"000",
                    "CashDiscPer1"	=>	"0,00",
                    "CashDiscPer2"	=>	"0,00"
                ),
                array(
                    "PayTerm"		=>	"2",
                    "PayDays"		=>	"000",
                    "CashDiscDays1"	=>	"000",
                    "CashDiscDays2"	=>	"000",
                    "CashDiscPer1"	=>	"0,00",
                    "CashDiscPer2"	=>	"0,00"
                )
            )
        ),
        "NetAmountByTurnOverTax" => array(
            array(
                "TurnOverTaxFree"	=>	"145,72",
                "TurnOverTax1"		=>	"0,00",
                "TurnOverTax2"		=>	"0,00",
                "TurnOverTax3"		=>	"0,00",
                "TurnOverTax4"		=>	"0,00",
                "TurnOverTax5"		=>	"0,00",
                "TurnOverTax6"		=>	"0,00",
                "TurnOverTax7"		=>	"0,00",
                "TurnOverTax8"		=>	"0,00"
            ),
        ),
        "GrossAmount"			=>	"145,72",
        "ProceedAmount"			=>	"145,72",
        "NetAmountByPayTerm2" => array(
            array(
                "Sum0"			=>	"0,00",
                "Sum1"			=>	"0,00",
                "Sum2"			=>	"0,00",
                "Sum3"			=>	"0,00",
                "Sum4"			=>	"0,00",
                "Sum5"			=>	"0,00",
                "Sum6"			=>	"0,00",
                "Sum7"			=>	"0,00",
                "Sum8"			=>	"0,00"
            ),
        ),
        "TaxCodes" => array(
            array(
                "TaxCode1"		=>	"0",
                "TaxCode2"		=>	"0",
                "TaxCode3"		=>	"0",
                "TaxCode4"		=>	"0",
                "TaxCode5"		=>	"0",
                "TaxCode6"		=>	"0",
                "TaxCode7"		=>	"0",
                "TaxCode8"		=>	"0"
            )
        )
    );
    $docs = array(
        array(
            "Version"			=>	"1.0",
            "ProducerName"			=>	"xxxxxxxx",
            "ProductName"			=>	"Classic Line",
            "xmlns"				=>	"x-schema:CL310_DezABFSchema.XML"
        ),
        "Company" => array(
            array(
                "MandateNumber"		=>	"111",
                "MandateName"  		=>	"xxx xxxxxxx-xxxxx xxxxxxx",
                "MandateCurr"  		=>	"EUR"
            ),
        ),
        "Doc" => $order
    );

    dump2xml($docs);


function dump2xml($array) {
    // output of this goes through 2xml
    array_walk($array, "gen_xml", "/Docs/");
}
function gen_xml($val, $key, $prefix)
{
    global $xml_fd;
    print "gen_xml(prefix=$prefix)\n";
    if (is_array($val)) {
    if (preg_match('/^\d+$/', $key)) {
        if ($key == 1) {
        print substr($prefix, 0, -1) . "\n";
        }
        array_walk($val, "gen_xml", "$prefix@");
    }
    else {
        array_walk($val, "gen_xml", "$prefix$key/");
    }
    }
    else {
        if (strlen($val) > 0)
        print "$prefix$key=$val\n";
        else
        print "$prefix$key\n";
    }
    print "gen_xml(prefix=$prefix) end\n";
}
echo "Done\n";
?>
--EXPECT--
gen_xml(prefix=/Docs/)
gen_xml(prefix=/Docs/@)
/Docs/@Version=1.0
gen_xml(prefix=/Docs/@) end
gen_xml(prefix=/Docs/@)
/Docs/@ProducerName=xxxxxxxx
gen_xml(prefix=/Docs/@) end
gen_xml(prefix=/Docs/@)
/Docs/@ProductName=Classic Line
gen_xml(prefix=/Docs/@) end
gen_xml(prefix=/Docs/@)
/Docs/@xmlns=x-schema:CL310_DezABFSchema.XML
gen_xml(prefix=/Docs/@) end
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
gen_xml(prefix=/Docs/Company/)
gen_xml(prefix=/Docs/Company/@)
/Docs/Company/@MandateNumber=111
gen_xml(prefix=/Docs/Company/@) end
gen_xml(prefix=/Docs/Company/@)
/Docs/Company/@MandateName=xxx xxxxxxx-xxxxx xxxxxxx
gen_xml(prefix=/Docs/Company/@) end
gen_xml(prefix=/Docs/Company/@)
/Docs/Company/@MandateCurr=EUR
gen_xml(prefix=/Docs/Company/@) end
gen_xml(prefix=/Docs/Company/) end
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/DocID=1
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/DocDate=19.09.06
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/ReSubmissionDate
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/DocTyp=Stapelauftrag
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/CustID=00000
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/CustomerAddress/)
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@Name1=name1
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@Name2=name2
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@Name3
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@City=city
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@Street=street
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@Postal=postcode
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@IATA=90
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@Country=Deutschland
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@ShortName=short
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@ContactKey
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/@)
/Docs/Doc/CustomerAddress/@EMail=email@example.com
gen_xml(prefix=/Docs/Doc/CustomerAddress/@) end
gen_xml(prefix=/Docs/Doc/CustomerAddress/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Text1
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Text2
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Wildcard1
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Wildcard2
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Dispatch=Paketdienst
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Weight=0,0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/BillingCustID=4300200000
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/ExtDocNr=00000000003
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/AnalysisLock
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/PrintFlag
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/FormType=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Curr=EUR
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/ExChangeRate=1,0000
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/WIRRate=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/OneTimeCustomer/)
gen_xml(prefix=/Docs/Doc/OneTimeCustomer/@)
/Docs/Doc/OneTimeCustomer/@BankCode
gen_xml(prefix=/Docs/Doc/OneTimeCustomer/@) end
gen_xml(prefix=/Docs/Doc/OneTimeCustomer/@)
/Docs/Doc/OneTimeCustomer/@BankAccount
gen_xml(prefix=/Docs/Doc/OneTimeCustomer/@) end
gen_xml(prefix=/Docs/Doc/OneTimeCustomer/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/Language=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/PriceGroup=1
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/PrFlag=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/SalesTaxKey=1
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/ProceedKey=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/CustDiscountGroup=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/Discount/)
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@FinDisc1=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@Disc1Base=145,72
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@Disc1Value=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@FinDisc2=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@Disc2Base=145,72
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@Disc2Value=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@FinDisc3=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@Disc3Base=145,72
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@Disc3Value=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/@)
/Docs/Doc/Discount/@ValueSummary=0,00
gen_xml(prefix=/Docs/Doc/Discount/@) end
gen_xml(prefix=/Docs/Doc/Discount/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/Contact/)
gen_xml(prefix=/Docs/Doc/Contact/@)
/Docs/Doc/Contact/@Repr=999
gen_xml(prefix=/Docs/Doc/Contact/@) end
gen_xml(prefix=/Docs/Doc/Contact/@)
/Docs/Doc/Contact/@Region=99
gen_xml(prefix=/Docs/Doc/Contact/@) end
gen_xml(prefix=/Docs/Doc/Contact/@)
/Docs/Doc/Contact/@Commission=0,00
gen_xml(prefix=/Docs/Doc/Contact/@) end
gen_xml(prefix=/Docs/Doc/Contact/@)
/Docs/Doc/Contact/@Agent=000000
gen_xml(prefix=/Docs/Doc/Contact/@) end
gen_xml(prefix=/Docs/Doc/Contact/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/Booking/)
gen_xml(prefix=/Docs/Doc/Booking/@)
/Docs/Doc/Booking/@CostUnit=0000000000
gen_xml(prefix=/Docs/Doc/Booking/@) end
gen_xml(prefix=/Docs/Doc/Booking/@)
/Docs/Doc/Booking/@CostCentre=0000000000
gen_xml(prefix=/Docs/Doc/Booking/@) end
gen_xml(prefix=/Docs/Doc/Booking/@)
/Docs/Doc/Booking/@AccountingArea=01
gen_xml(prefix=/Docs/Doc/Booking/@) end
gen_xml(prefix=/Docs/Doc/Booking/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/InvoiceCycleKey=0
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/AnalysisKey
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/OrderNumber
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/OrderDate
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/OrderCode
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/DocItems/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/PosType=1
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/ItemRef=1002
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/CRef
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/Desc1=Pr�sentation Niederlande per
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/Desc2
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/ArticleGroup=102
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/PosTypeVersion=E
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/@)
/Docs/Doc/DocItems/DocItem/Delivery/@DelWeek
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/@)
/Docs/Doc/DocItems/DocItem/Delivery/@DelDay
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/@)
/Docs/Doc/DocItems/DocItem/Delivery/@DelTime
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Delivery/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/PricePu=145,72
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/PriceUnit=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/PriceCalculation=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/ItemVal=145,72
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/InputKey=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/AveragePurchasePrice=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@)
/Docs/Doc/DocItems/DocItem/Tax/@TaxCode=00
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@)
/Docs/Doc/DocItems/DocItem/Tax/@TaxBra=000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@)
/Docs/Doc/DocItems/DocItem/Tax/@TaxBraAccess=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@)
/Docs/Doc/DocItems/DocItem/Tax/@TaxSumIndex=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Tax/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/DiscountArticle/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/DiscountArticle/@)
/Docs/Doc/DocItems/DocItem/DiscountArticle/@DiscPC=0,00
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/DiscountArticle/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/DiscountArticle/@)
/Docs/Doc/DocItems/DocItem/DiscountArticle/@DiscKey=1
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/DiscountArticle/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/DiscountArticle/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/ProceedKeyArticle=01
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/ActionKey=00
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/ContactCommissionArticle=0,00
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/QuantdependentPriceKey
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/Quant=1
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/QuantUnit
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@)
/Docs/Doc/DocItems/DocItem/Meas/@Count=1
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@)
/Docs/Doc/DocItems/DocItem/Meas/@Length=0,000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@)
/Docs/Doc/DocItems/DocItem/Meas/@Width=0,000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@)
/Docs/Doc/DocItems/DocItem/Meas/@Height=0,000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/Meas/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/DecimalPlace=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/MultiplierQuant=1,000000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/DifferingQuantUnit
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/DecimalPlaceConversion=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/WeightArticle/)
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/WeightArticle/@)
/Docs/Doc/DocItems/DocItem/WeightArticle/@Amount=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/WeightArticle/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/WeightArticle/@)
/Docs/Doc/DocItems/DocItem/WeightArticle/@Unit=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/WeightArticle/@) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/WeightArticle/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/Wreath=0,000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/Stock=1
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/CostUnitArticle
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/SerialNbKey=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/TextComplementKey=0
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/PartsListPrintKey
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/)
/Docs/Doc/DocItems/DocItem/Prod=0000000000
gen_xml(prefix=/Docs/Doc/DocItems/DocItem/) end
gen_xml(prefix=/Docs/Doc/DocItems/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/Payment/)
/Docs/Doc/Payment/PaymentKey=0
gen_xml(prefix=/Docs/Doc/Payment/) end
gen_xml(prefix=/Docs/Doc/Payment/)
/Docs/Doc/Payment/ReminderKey=00
gen_xml(prefix=/Docs/Doc/Payment/) end
gen_xml(prefix=/Docs/Doc/Payment/)
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/)
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@PayTerm=1
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@PayDays=000
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscDays1=000
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscDays2=000
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscPer1=0,00
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscPer2=0,00
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/)
/Docs/Doc/Payment/PayTerms
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@PayTerm=2
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@PayDays=000
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscDays1=000
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscDays2=000
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscPer1=0,00
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@)
/Docs/Doc/Payment/PayTerms/@CashDiscPer2=0,00
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/@) end
gen_xml(prefix=/Docs/Doc/Payment/PayTerms/) end
gen_xml(prefix=/Docs/Doc/Payment/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/)
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTaxFree=145,72
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax1=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax2=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax3=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax4=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax5=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax6=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax7=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@)
/Docs/Doc/NetAmountByTurnOverTax/@TurnOverTax8=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByTurnOverTax/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/GrossAmount=145,72
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
/Docs/Doc/ProceedAmount=145,72
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/)
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum0=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum1=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum2=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum3=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum4=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum5=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum6=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum7=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@)
/Docs/Doc/NetAmountByPayTerm2/@Sum8=0,00
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/@) end
gen_xml(prefix=/Docs/Doc/NetAmountByPayTerm2/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/Doc/)
gen_xml(prefix=/Docs/Doc/TaxCodes/)
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode1=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode2=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode3=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode4=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode5=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode6=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode7=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/@)
/Docs/Doc/TaxCodes/@TaxCode8=0
gen_xml(prefix=/Docs/Doc/TaxCodes/@) end
gen_xml(prefix=/Docs/Doc/TaxCodes/) end
gen_xml(prefix=/Docs/Doc/) end
gen_xml(prefix=/Docs/) end
Done
