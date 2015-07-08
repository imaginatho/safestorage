# @(#) Fuente : safe_storage.mk v. vlib#10 [ 20-JAN-2011 11:58:30 ]
#
# safe_storage.mk
#
# @(#) Descripcion : none
#
# Creado por desar7, Thu Feb 21 11:14:46 2002
#
# (c) CIRSA INTERACTIVE Corp.
# Dpto. Desarrollo. Madrid
#
# Descripcion larga del fichero
#
# Construye el binario del proceso safe_storage
#
####################################################################################
# Este make utiliza las siguientes definiciones:
#
# - _DEBUG_      para visualizar determinadas alarmas
#
####################################################################################

# Nombre del Makefile
MAKE_NAME=safe_test.mk

# Producto
PRODUCTO=safe_test

# Flags para el propio Makefile
OWN_CCFLAGS=-D_DEBUG_LEVEL_=2 -I ./include -I ../include
OWN_LDFLAGS=-lxerces-c


# Tipo de producto
TIPO_PRODUCTO=EJECUTABLE

# Ficheros objeto para construir el producto
OBJS = safe_test.o

#LIBS=libpkm libcrm libthm libcrm libcmn libUtilCpp libgols libpthread libsafestorage libcitcrypto libssl
#LIBS=libpkm libcrm libcmn libthm libUtilCpp libgols libpthread libsafestorage libcitcrypto libssl
# original
# LIBS=libgols libsafestorage libpkm libcrm libcmn libthm libUtilCpp libcitcrypto libssl
LIBS=libsafestorage libpkm libcrm libcmn libthm libUtilCpp libgols libcitcrypto libssl

# libpkm libcrm libcmn libthm libUtilCpp libsafestorage libcitcrypto libssl

# Inclusion fichero comun de reglas para Sistema Central
include $(GOLMK)/CirsaRules.mk
