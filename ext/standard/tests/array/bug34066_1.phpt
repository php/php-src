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
    array_walk_recursive($array, "gen_xml", "/Docs/");
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
	    array_walk_recursive($val, "gen_xml", "$prefix@");
	}
	else {
	    array_walk_recursive($val, "gen_xml", "$prefix$key/");
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
--EXPECTF--
gen_xml(prefix=/Docs/)
/Docs/Version=1.0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ProducerName=xxxxxxxx
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ProductName=Classic Line
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/xmlns=x-schema:CL310_DezABFSchema.XML
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/MandateNumber=111
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/MandateName=xxx xxxxxxx-xxxxx xxxxxxx
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/MandateCurr=EUR
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DocID=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DocDate=19.09.06
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ReSubmissionDate
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DocTyp=Stapelauftrag
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CustID=00000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Name1=name1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Name2=name2
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Name3
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/City=city
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Street=street
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Postal=postcode
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/IATA=90
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Country=Deutschland
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ShortName=short
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ContactKey
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/EMail=email@example.com
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Text1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Text2
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Wildcard1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Wildcard2
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Dispatch=Paketdienst
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Weight=0,0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/BillingCustID=4300200000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ExtDocNr=00000000003
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/AnalysisLock
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PrintFlag
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/FormType=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Curr=EUR
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ExChangeRate=1,0000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/WIRRate=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/BankCode
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/BankAccount
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Language=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PriceGroup=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PrFlag=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/SalesTaxKey=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ProceedKey=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CustDiscountGroup=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/FinDisc1=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Disc1Base=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Disc1Value=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/FinDisc2=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Disc2Base=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Disc2Value=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/FinDisc3=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Disc3Base=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Disc3Value=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ValueSummary=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Repr=999
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Region=99
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Commission=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Agent=000000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CostUnit=0000000000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CostCentre=0000000000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/AccountingArea=01
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/InvoiceCycleKey=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/AnalysisKey
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/OrderNumber
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/OrderDate
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/OrderCode
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PosType=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ItemRef=1002
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CRef
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Desc1=Pr�sentation Niederlande per
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Desc2
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ArticleGroup=102
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PosTypeVersion=E
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DelWeek
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DelDay
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DelTime
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PricePu=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PriceUnit=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PriceCalculation=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ItemVal=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/InputKey=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/AveragePurchasePrice=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode=00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxBra=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxBraAccess=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxSumIndex=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DiscPC=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DiscKey=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ProceedKeyArticle=01
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ActionKey=00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ContactCommissionArticle=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/QuantdependentPriceKey
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Quant=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/QuantUnit
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Count=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Length=0,000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Width=0,000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Height=0,000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DecimalPlace=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/MultiplierQuant=1,000000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DifferingQuantUnit
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/DecimalPlaceConversion=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Amount=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Unit=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Wreath=0,000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Stock=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CostUnitArticle
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/SerialNbKey=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TextComplementKey=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PartsListPrintKey
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Prod=0000000000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PaymentKey=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ReminderKey=00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PayTerm=1
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PayDays=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscDays1=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscDays2=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscPer1=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscPer2=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PayTerm=2
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/PayDays=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscDays1=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscDays2=000
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscPer1=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/CashDiscPer2=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTaxFree=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax1=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax2=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax3=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax4=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax5=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax6=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax7=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TurnOverTax8=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/GrossAmount=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/ProceedAmount=145,72
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum0=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum1=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum2=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum3=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum4=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum5=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum6=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum7=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/Sum8=0,00
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode1=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode2=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode3=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode4=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode5=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode6=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode7=0
gen_xml(prefix=/Docs/) end
gen_xml(prefix=/Docs/)
/Docs/TaxCode8=0
gen_xml(prefix=/Docs/) end
Done
