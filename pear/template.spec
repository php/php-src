Summary: PEAR: @summary@
Name: @rpm_package@
Version: @version@
Release: 1
License: @release_license@
Group: Development/Libraries
Source: http://@master_server@/get/@package@-%{version}.tgz
BuildRoot: %{_tmppath}/%{name}-root
URL: http://@master_server@/
Prefix: %{_prefix}
#Docdir: @doc_dir@/@package@
BuildArchitectures: @arch@
@extra_headers@

%description
@description@

%prep
rm -rf %{buildroot}/*
# XXX Source files location is missing here in pear cmd
pear -v -c %{buildroot}/pearrc \
        -d php_dir=%{_libdir}/php/pear \
        -d doc_dir=/docs \
        -d bin_dir=%{_bindir} \
        -d data_dir=%{_libdir}/php/pear/data \
        -d test_dir=%{_libdir}/php/pear/tests \
        -d ext_dir=%{_libdir} \
        -s

%build
echo BuildRoot=%{buildroot}

%clean
[ -n "%{buildroot}" -a "%{buildroot}" != / ] && rm -rf %{buildroot}

%postun
pear uninstall --nodeps -r @package@

%post
pear install --nodeps -r @rpm_xml_dir@/@package@.xml

%install
pear -c "%{buildroot}/pearrc" install --nodeps -R "%{buildroot}" \
        "$RPM_SOURCE_DIR/@package@-%{version}.tgz"
rm %{buildroot}/pearrc
rm %{buildroot}/%{_libdir}/php/pear/.filemap
rm %{buildroot}/%{_libdir}/php/pear/.lock
rm -rf %{buildroot}/%{_libdir}/php/pear/.registry
for DOCDIR in docs doc examples; do
    if [ -d "%{buildroot}/docs/@package@/$DOCDIR" ]; then
        rm -rf $RPM_BUILD_DIR/$DOCDIR
        mv %{buildroot}/docs/@package@/$DOCDIR $RPM_BUILD_DIR
        rm -rf %{buildroot}/docs
    fi
done
mkdir -p %{buildroot}@rpm_xml_dir@
tar -xzf $RPM_SOURCE_DIR/@package@-%{version}.tgz package.xml
cp -p package.xml %{buildroot}@rpm_xml_dir@/@package@.xml

#rm -rf %{buildroot}/*
#pear -q install -R %{buildroot} -n package.xml
#mkdir -p %{buildroot}@rpm_xml_dir@
#cp -p package.xml %{buildroot}@rpm_xml_dir@/@package@.xml

%files
    %defattr(-,root,root)
    %doc @doc_files@
    /
