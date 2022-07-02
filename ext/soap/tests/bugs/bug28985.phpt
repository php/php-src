--TEST--
Bug #28985 (__getTypes() returning nothing on complex WSDL)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SOAPClient(__DIR__.'/bug28985.wsdl', array('trace'=>1));
var_dump($client->__getTypes());
?>
--EXPECT--
array(42) {
  [0]=>
  string(100) "struct LoginMGDIS {
 string iUserLogin;
 string iUserId;
 string iUserPassword;
 string iProfilId;
}"
  [1]=>
  string(29) "struct LoginMGDISResponse {
}"
  [2]=>
  string(28) "struct GetIdentification {
}"
  [3]=>
  string(77) "struct GetIdentificationResponse {
 ArrayOfAnyType GetIdentificationResult;
}"
  [4]=>
  string(43) "struct ArrayOfAnyType {
 anyType anyType;
}"
  [5]=>
  string(37) "struct RollbackCurrentTransaction {
}"
  [6]=>
  string(45) "struct RollbackCurrentTransactionResponse {
}"
  [7]=>
  string(68) "struct GetListeProfil {
 string iUserLogin;
 string iUserPassword;
}"
  [8]=>
  string(87) "struct MGCodeLibelle {
 string Code;
 string Libelle;
 boolean Default;
 anyType Tag;
}"
  [9]=>
  string(61) "struct ArrayOfMGCodeLibelle {
 MGCodeLibelle MGCodeLibelle;
}"
  [10]=>
  string(77) "struct GetListeProfilResponse {
 ArrayOfMGCodeLibelle GetListeProfilResult;
}"
  [11]=>
  string(41) "struct GetListeValCodif {
 string Code;
}"
  [12]=>
  string(43) "struct ArrayOfMGCodif {
 MGCodif MGCodif;
}"
  [13]=>
  string(18) "struct MGCodif {
}"
  [14]=>
  string(75) "struct GetListeValCodifResponse {
 ArrayOfMGCodif GetListeValCodifResult;
}"
  [15]=>
  string(39) "struct TestPhpSoap {
 MGCodif entree;
}"
  [16]=>
  string(57) "struct TestPhpSoapResponse {
 string TestPhpSoapResult;
}"
  [17]=>
  string(50) "struct GetListeCodif {
 boolean iGetListeValeur;
}"
  [18]=>
  string(87) "struct MGCodifGrp {
 string TypeCodif;
 string LibCodif;
 ArrayOfMGCodif ListeCodifs;
}"
  [19]=>
  string(52) "struct ArrayOfMGCodifGrp {
 MGCodifGrp MGCodifGrp;
}"
  [20]=>
  string(72) "struct GetListeCodifResponse {
 ArrayOfMGCodifGrp GetListeCodifResult;
}"
  [21]=>
  string(57) "struct DroitCreation {
 string iObjet;
 string iProfil;
}"
  [22]=>
  string(62) "struct DroitCreationResponse {
 boolean DroitCreationResult;
}"
  [23]=>
  string(74) "struct ListeDroitCreation {
 ArrayOfString iListeObjet;
 string iProfil;
}"
  [24]=>
  string(40) "struct ArrayOfString {
 string string;
}"
  [25]=>
  string(79) "struct ListeDroitCreationResponse {
 ArrayOfAnyType ListeDroitCreationResult;
}"
  [26]=>
  string(87) "struct GetDroitsObjetProtege {
 string iObjet;
 string iProfil;
 string iUtilisateur;
}"
  [27]=>
  string(154) "struct MGDroitsObjetProtege {
 string LbUti;
 string LbProf;
 string LbServ;
 string LbDir;
 boolean isProtected;
 ArrayOfMGDroitAcces ListeDroitsAcces;
}"
  [28]=>
  string(58) "struct ArrayOfMGDroitAcces {
 MGDroitAcces MGDroitAcces;
}"
  [29]=>
  string(104) "struct MGDroitAcces {
 string IdProfil;
 boolean Lecture;
 boolean Modification;
 boolean Suppression;
}"
  [30]=>
  string(91) "struct GetDroitsObjetProtegeResponse {
 MGDroitsObjetProtege GetDroitsObjetProtegeResult;
}"
  [31]=>
  string(76) "struct GetPrivileges {
 string iIdSupport;
 int iIdForme;
 string iProfil;
}"
  [32]=>
  string(68) "struct GetPrivilegesResponse {
 ArrayOfString GetPrivilegesResult;
}"
  [33]=>
  string(46) "struct GetLibelleProfil {
 string iIdProfil;
}"
  [34]=>
  string(67) "struct GetLibelleProfilResponse {
 string GetLibelleProfilResult;
}"
  [35]=>
  string(91) "struct GetValeurRecherche {
 string iChampSource;
 string iTable;
 string iOrderByClause;
}"
  [36]=>
  string(78) "struct GetValeurRechercheResponse {
 ArrayOfString GetValeurRechercheResult;
}"
  [37]=>
  string(128) "struct GetValeurRechercheWithClauseWhere {
 string iChampSource;
 string iTable;
 string iClauseWhere;
 string iOrderByClause;
}"
  [38]=>
  string(108) "struct GetValeurRechercheWithClauseWhereResponse {
 ArrayOfString GetValeurRechercheWithClauseWhereResult;
}"
  [39]=>
  string(27) "struct GetEnvironnement {
}"
  [40]=>
  string(106) "struct MGEnvironnement {
 string RepBureautique;
 string RepBureautiqueImage;
 string RepBureautiqueDoc;
}"
  [41]=>
  string(76) "struct GetEnvironnementResponse {
 MGEnvironnement GetEnvironnementResult;
}"
}
