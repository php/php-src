#!/bin/sh
set -e

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

if test -f "/etc/ldap/schema/ppolicy.ldif"; then
  sudo ldapadd -Q -Y EXTERNAL -H ldapi:/// -f /etc/ldap/schema/ppolicy.ldif
fi

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
add: olcTLSProtocolMin
olcTLSProtocolMin: 3.3
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

sudo ldapmodify -Q -Y EXTERNAL -H ldapi:/// << EOF
dn: $DBDN
changetype: modify
add: olcDbIndex
olcDbIndex: entryExpireTimestamp eq
EOF

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

sudo service slapd restart


echo "=== TLS DEBUG: Testing TLS_PROTOCOL_MAX 3.2 ==="
TEMP_LDAP_CONF=$(mktemp)
echo 'TLS_PROTOCOL_MAX 3.2' > "$TEMP_LDAP_CONF"

echo "Debug info:"
echo "  Temp config file: $TEMP_LDAP_CONF"
echo "  Contents:"
cat "$TEMP_LDAP_CONF"
echo ""

echo "  Server TLS configuration check:"
sudo ldapsearch -Q -LLL -Y EXTERNAL -H ldapi:/// -b cn=config '(olcTLSProtocolMin=*)' dn olcTLSProtocolMin 2>/dev/null || echo "  No TLS protocol min configured"

echo ""
echo "Checking OpenLDAP config priority order (settings later in list override earlier ones):"

echo "1. LDAPNOINIT: ${LDAPNOINIT:-not set}"

echo "2. System file /etc/openldap/ldap.conf:"
if [ -f /etc/openldap/ldap.conf ]; then
  echo "   EXISTS - Contents:"
  cat /etc/openldap/ldap.conf
  sudo rm /etc/openldap/ldap.conf
  echo "   REMOVED to test priority"
else
  echo "   Does not exist"
fi

echo "3. User files:"
echo "   \$HOME/ldaprc: $([ -f "$HOME/ldaprc" ] && echo "EXISTS" || echo "does not exist")"
if [ -f "$HOME/ldaprc" ]; then cat "$HOME/ldaprc"; fi
echo "   \$HOME/.ldaprc: $([ -f "$HOME/.ldaprc" ] && echo "EXISTS" || echo "does not exist")"
if [ -f "$HOME/.ldaprc" ]; then cat "$HOME/.ldaprc"; fi
echo "   ./ldaprc: $([ -f "./ldaprc" ] && echo "EXISTS" || echo "does not exist")"
if [ -f "./ldaprc" ]; then cat "./ldaprc"; fi

echo "4. System file \$LDAPCONF: ${LDAPCONF:-not set}"
if [ -n "$LDAPCONF" ] && [ -f "$LDAPCONF" ]; then
  echo "   Contents:"
  cat "$LDAPCONF"
fi

echo "5. User files \$LDAPRC related:"
echo "   \$LDAPRC: ${LDAPRC:-not set}"
if [ -n "$LDAPRC" ]; then
  echo "   \$HOME/\$LDAPRC: $([ -f "$HOME/$LDAPRC" ] && echo "EXISTS" || echo "does not exist")"
  echo "   \$HOME/.\$LDAPRC: $([ -f "$HOME/.$LDAPRC" ] && echo "EXISTS" || echo "does not exist")"
  echo "   ./\$LDAPRC: $([ -f "./$LDAPRC" ] && echo "EXISTS" || echo "does not exist")"
fi

echo "6. Environment variables \$LDAP<OPTION>:"
echo "   \$LDAPTLS_PROTOCOL_MAX: ${LDAPTLS_PROTOCOL_MAX:-not set}"
echo "   \$LDAPBASE: ${LDAPBASE:-not set}"
echo "   \$LDAPURI: ${LDAPURI:-not set}"

echo ""
echo "Testing with our LDAPCONF setting:"
echo "  Command: LDAPCONF=\"$TEMP_LDAP_CONF\" ldapsearch -H ldap://localhost -D cn=Manager,dc=my-domain,dc=com -w secret -s base -b dc=my-domain,dc=com -ZZ 'objectclass=*'"
LDAPCONF="$TEMP_LDAP_CONF" ldapsearch -H ldap://localhost -D cn=Manager,dc=my-domain,dc=com -w secret -s base -b dc=my-domain,dc=com -Z 'objectclass=*' >/dev/null 2>&1
result1=$?
echo "  Result: $([ $result1 -eq 0 ] && echo 'SUCCESS (unexpected)' || echo 'FAILED (expected)')"

echo ""
echo "Testing with environment variable (highest priority):"
echo "  Command: LDAPTLS_PROTOCOL_MAX=3.2 ldapsearch -H ldap://localhost -D cn=Manager,dc=my-domain,dc=com -w secret -s base -b dc=my-domain,dc=com -ZZ 'objectclass=*'"
LDAPTLS_PROTOCOL_MAX=3.2 ldapsearch -H ldap://localhost -D cn=Manager,dc=my-domain,dc=com -w secret -s base -b dc=my-domain,dc=com -Z 'objectclass=*' >/dev/null 2>&1
result2=$?
echo "  Result: $([ $result2 -eq 0 ] && echo 'SUCCESS (unexpected)' || echo 'FAILED (expected)')"

echo ""
echo "Testing what TLS version is actually being negotiated:"
echo | openssl s_client -connect localhost:636 -servername localhost 2>&1 | grep -E "(Protocol|Cipher)" || echo "  LDAPS connection failed"

rm -f "$TEMP_LDAP_CONF"
echo ""
echo "Summary:"
echo "- If both LDAPCONF and LDAPTLS_PROTOCOL_MAX tests succeed, TLS version restrictions are not working"
echo "- This would explain why the PHP test expecting failures gets successes instead"

exit 1

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

