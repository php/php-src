<?php

file_put_contents("{LOG_FILE}", var_export(getallheaders(), true));

echo <<<END
<?xml version="1.0" encoding="utf-8"?>
<definitions xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns="http://schemas.xmlsoap.org/wsdl/">
  <portType name="soapServicePortType" />
  <binding name="soapServiceBinding" type="typens:soapServicePortType" />
  <service name="soapServiceService">
    <port name="soapServicePort" binding="typens:soapServiceBinding">
      <soap:address location="###PHP_SELF###" />
    </port>
  </service>
</definitions>
END;