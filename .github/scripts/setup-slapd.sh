#!/bin/sh
set -ex

# Create TLS certificate
sudo mkdir -p /etc/ldap/ssl

alt_names() {
  (
      (
        (hostname && hostname -a && hostname -A && hostname -f) |
        xargs -n 1 |
        sort -u |
        sed -e 's/\(\S\+\)/DNS:\1/g'
      ) && (
        (hostname -i && hostname -I && echo "127.0.0.1 ::1") |
        xargs -n 1 |
        sort -u |
        sed -e 's/\(\S\+\)/IP:\1/g'
      )
  ) | paste -d, -s
}

sudo openssl req -newkey rsa:4096 -x509 -nodes -days 3650 \
  -out /etc/ldap/ssl/server.crt -keyout /etc/ldap/ssl/server.key \
  -subj "/C=US/ST=Arizona/L=Localhost/O=localhost/CN=localhost" \
  -addext "subjectAltName = `alt_names`"

sudo chown -R openldap:openldap /etc/ldap/ssl

# Display the TLS certificate (should be world readable)
openssl x509 -noout -text -in /etc/ldap/ssl/server.crt

# Point to the certificate generated
if ! grep -q 'TLS_CACERT \/etc\/ldap\/ssl\/server.crt' /etc/ldap/ldap.conf; then
  sudo sed -e 's|^\s*TLS_CACERT|# TLS_CACERT|' -i /etc/ldap/ldap.conf
  echo 'TLS_CACERT /etc/ldap/ssl/server.crt' | sudo tee -a /etc/ldap/ldap.conf
fi

# Configure LDAP protocols to serve.
sudo sed -e 's|^\s*SLAPD_SERVICES\s*=.*$|SLAPD_SERVICES="ldap:/// ldaps:/// ldapi:///"|' -i /etc/default/slapd

# Configure LDAP database.
DBDN=`sudo ldapsearch -Q -LLL -Y EXTERNAL -H ldapi:/// -b cn=config '(&(olcRootDN=*)(olcSuffix=*))' dn | grep -i '^dn:' | sed -e 's/^dn:\s*//'`;

sudo ldapadd -Q -Y EXTERNAL -H ldapi:/// -f /etc/ldap/schema/ppolicy.ldif

sudo service slapd restart

sudo ldapmodify -Q -Y EXTERNAL -H ldapi:/// << EOF
dn: $DBDN
changetype: modify
replace: olcSuffix
olcSuffix: dc=my-domain,dc=com
-
replace: olcRootDN
olcRootDN: cn=Manager,dc=my-domain,dc=com
-
replace: olcRootPW
olcRootPW: secret

dn: cn=config
changetype: modify
add: olcTLSCACertificateFile
olcTLSCACertificateFile: /etc/ldap/ssl/server.crt
-
add: olcTLSCertificateFile
olcTLSCertificateFile: /etc/ldap/ssl/server.crt
-
add: olcTLSCertificateKeyFile
olcTLSCertificateKeyFile: /etc/ldap/ssl/server.key
-
add: olcTLSVerifyClient
olcTLSVerifyClient: never
-
add: olcAuthzRegexp
olcAuthzRegexp: uid=usera,cn=digest-md5,cn=auth cn=usera,dc=my-domain,dc=com
-
replace: olcLogLevel
olcLogLevel: -1

dn: cn=module{0},cn=config
changetype: modify
add: olcModuleLoad
olcModuleLoad: sssvlv
-
add: olcModuleLoad
olcModuleLoad: ppolicy
-
add: olcModuleLoad
olcModuleLoad: dds
EOF

sudo service slapd restart

sudo ldapadd -Q -Y EXTERNAL -H ldapi:/// << EOF
dn: olcOverlay=sssvlv,$DBDN
objectClass: olcOverlayConfig
objectClass: olcSssVlvConfig
olcOverlay: sssvlv
olcSssVlvMax: 10
olcSssVlvMaxKeys: 5

dn: olcOverlay=ppolicy,$DBDN
objectClass: olcOverlayConfig
objectClass: olcPPolicyConfig
olcOverlay: ppolicy
### This would clutter our DIT and make tests to fail, while ppolicy does not
### seem to work as we expect (it does not seem to provide expected controls)
## olcPPolicyDefault: cn=default,ou=pwpolicies,dc=my-domain,dc=com
## olcPPolicyHashCleartext: FALSE
## olcPPolicyUseLockout: TRUE

dn: olcOverlay=dds,$DBDN
objectClass: olcOverlayConfig
objectClass: olcDdsConfig
olcOverlay: dds
EOF

sudo service slapd restart

sudo ldapmodify -Q -Y EXTERNAL -H ldapi:/// << EOF
dn: $DBDN
changetype: modify
add: olcDbIndex
olcDbIndex: entryExpireTimestamp eq
EOF

sudo service slapd restart

ldapadd -H ldapi:/// -D cn=Manager,dc=my-domain,dc=com -w secret <<EOF
dn: dc=my-domain,dc=com
objectClass: top
objectClass: organization
objectClass: dcObject
dc: my-domain
o: php ldap tests

### This would clutter our DIT and make tests to fail, while ppolicy does not
### seem to work as we expect (it does not seem to provide expected controls)
## dn: ou=pwpolicies,dc=my-domain,dc=com
## objectClass: top
## objectClass: organizationalUnit
## ou: pwpolicies
##
## dn: cn=default,ou=pwpolicies,dc=my-domain,dc=com
## objectClass: top
## objectClass: person
## objectClass: pwdPolicy
## cn: default
## sn: default
## pwdAttribute: userPassword
## pwdMaxAge: 2592000
## pwdExpireWarning: 3600
## #pwdInHistory: 0
## pwdCheckQuality: 0
## pwdMaxFailure: 5
## pwdLockout: TRUE
## #pwdLockoutDuration: 0
## #pwdGraceAuthNLimit: 0
## #pwdFailureCountInterval: 0
## pwdMustChange: FALSE
## pwdMinLength: 3
## pwdAllowUserChange: TRUE
## pwdSafeModify: FALSE
EOF

# Verify TLS connection
tries=0
while : ; do
	ldapsearch -d 255 -H ldaps://localhost -D cn=Manager,dc=my-domain,dc=com -w secret -s base -b dc=my-domain,dc=com 'objectclass=*'
	rt=$?
	if [ $rt -eq 0 ]; then
		echo "OK"
		exit 0
	else
		tries=$((tries+1))
		if [ $((tries)) -gt 3 ]; then
			echo "exit failure $rt"
			exit $rt
		else
			echo "trying again"
			sleep 3
		fi
	fi
done
