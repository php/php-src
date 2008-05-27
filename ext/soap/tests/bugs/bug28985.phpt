--TEST--
Bug #28985 (__getTypes() returning nothing on complex WSDL)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SOAPClient(dirname(__FILE__).'/bug28985.wsdl', array('trace'=>1));
var_dump($client->__getTypes());
?>
--EXPECT--
array(42) {
  [0]=>
  unicode(100) "struct LoginMGDIS {
 string iUserLogin;
 string iUserId;
 string iUserPassword;
 string iProfilId;
}"
  [1]=>
  unicode(29) "struct LoginMGDISResponse {
}"
  [2]=>
  unicode(28) "struct GetIdentification {
}"
  [3]=>
  unicode(77) "struct GetIdentificationResponse {
 ArrayOfAnyType GetIdentificationResult;
}"
  [4]=>
  unicode(43) "struct ArrayOfAnyType {
 anyType anyType;
}"
  [5]=>
  unicode(37) "struct RollbackCurrentTransaction {
}"
  [6]=>
  unicode(45) "struct RollbackCurrentTransactionResponse {
}"
  [7]=>
  unicode(68) "struct GetListeProfil {
 string iUserLogin;
 string iUserPassword;
}"
  [8]=>
  unicode(86) "struct MGCodeLibelle {
 string Code;
 string Libelle;
 boolean Defaut;
 anyType Tag;
}"
  [9]=>
  unicode(61) "struct ArrayOfMGCodeLibelle {
 MGCodeLibelle MGCodeLibelle;
}"
  [10]=>
  unicode(77) "struct GetListeProfilResponse {
 ArrayOfMGCodeLibelle GetListeProfilResult;
}"
  [11]=>
  unicode(41) "struct GetListeValCodif {
 string Code;
}"
  [12]=>
  unicode(43) "struct ArrayOfMGCodif {
 MGCodif MGCodif;
}"
  [13]=>
  unicode(18) "struct MGCodif {
}"
  [14]=>
  unicode(75) "struct GetListeValCodifResponse {
 ArrayOfMGCodif GetListeValCodifResult;
}"
  [15]=>
  unicode(39) "struct TestPhpSoap {
 MGCodif entree;
}"
  [16]=>
  unicode(57) "struct TestPhpSoapResponse {
 string TestPhpSoapResult;
}"
  [17]=>
  unicode(50) "struct GetListeCodif {
 boolean iGetListeValeur;
}"
  [18]=>
  unicode(87) "struct MGCodifGrp {
 string TypeCodif;
 string LibCodif;
 ArrayOfMGCodif ListeCodifs;
}"
  [19]=>
  unicode(52) "struct ArrayOfMGCodifGrp {
 MGCodifGrp MGCodifGrp;
}"
  [20]=>
  unicode(72) "struct GetListeCodifResponse {
 ArrayOfMGCodifGrp GetListeCodifResult;
}"
  [21]=>
  unicode(57) "struct DroitCreation {
 string iObjet;
 string iProfil;
}"
  [22]=>
  unicode(62) "struct DroitCreationResponse {
 boolean DroitCreationResult;
}"
  [23]=>
  unicode(74) "struct ListeDroitCreation {
 ArrayOfString iListeObjet;
 string iProfil;
}"
  [24]=>
  unicode(40) "struct ArrayOfString {
 string string;
}"
  [25]=>
  unicode(79) "struct ListeDroitCreationResponse {
 ArrayOfAnyType ListeDroitCreationResult;
}"
  [26]=>
  unicode(87) "struct GetDroitsObjetProtege {
 string iObjet;
 string iProfil;
 string iUtilisateur;
}"
  [27]=>
  unicode(154) "struct MGDroitsObjetProtege {
 string LbUti;
 string LbProf;
 string LbServ;
 string LbDir;
 boolean isProtected;
 ArrayOfMGDroitAcces ListeDroitsAcces;
}"
  [28]=>
  unicode(58) "struct ArrayOfMGDroitAcces {
 MGDroitAcces MGDroitAcces;
}"
  [29]=>
  unicode(104) "struct MGDroitAcces {
 string IdProfil;
 boolean Lecture;
 boolean Modification;
 boolean Suppression;
}"
  [30]=>
  unicode(91) "struct GetDroitsObjetProtegeResponse {
 MGDroitsObjetProtege GetDroitsObjetProtegeResult;
}"
  [31]=>
  unicode(76) "struct GetPrivileges {
 string iIdSupport;
 int iIdForme;
 string iProfil;
}"
  [32]=>
  unicode(68) "struct GetPrivilegesResponse {
 ArrayOfString GetPrivilegesResult;
}"
  [33]=>
  unicode(46) "struct GetLibelleProfil {
 string iIdProfil;
}"
  [34]=>
  unicode(67) "struct GetLibelleProfilResponse {
 string GetLibelleProfilResult;
}"
  [35]=>
  unicode(91) "struct GetValeurRecherche {
 string iChampSource;
 string iTable;
 string iOrderByClause;
}"
  [36]=>
  unicode(78) "struct GetValeurRechercheResponse {
 ArrayOfString GetValeurRechercheResult;
}"
  [37]=>
  unicode(128) "struct GetValeurRechercheWithClauseWhere {
 string iChampSource;
 string iTable;
 string iClauseWhere;
 string iOrderByClause;
}"
  [38]=>
  unicode(108) "struct GetValeurRechercheWithClauseWhereResponse {
 ArrayOfString GetValeurRechercheWithClauseWhereResult;
}"
  [39]=>
  unicode(27) "struct GetEnvironnement {
}"
  [40]=>
  unicode(106) "struct MGEnvironnement {
 string RepBureautique;
 string RepBureautiqueImage;
 string RepBureautiqueDoc;
}"
  [41]=>
  unicode(76) "struct GetEnvironnementResponse {
 MGEnvironnement GetEnvironnementResult;
}"
}
