# Set expected paths
export PATH="$(brew --prefix bison)/bin:$(brew --prefix ccache)/libexec:$PATH"
export PKG_CONFIG_PATH="$(brew --prefix openssl)/lib/pkgconfig:$(brew --prefix libedit)/lib/pkgconfig:$(brew --prefix zlib)/lib/pkgconfig:$PKG_CONFIG_PATH"
export CFLAGS="-Wno-nullability-completeness -Wno-expansion-to-defined $CFLAGS"
export CXXFLAGS="-Wno-nullability-completeness -Wno-expansion-to-defined $CXXFLAGS"
# Force relinking
brew link icu4c gettext --force
# Start DB services
brew services start mysql
brew services start postgresql
