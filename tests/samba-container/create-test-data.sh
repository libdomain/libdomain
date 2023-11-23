#!/bin/bash

echo 'password145Qw!' | kinit administrator@DOMAIN.ALT || :

samba-tool user create test_delete_user secretPWD123! --given-name=test --surname=delete --mail-address=test.user.delete@domain.alt --login-shell=/bin/bash -k yes
samba-tool user create test_mod_user secretPWD123! --given-name=test --surname=mod --mail-address=test.user.mod@domain.alt --login-shell=/bin/bash -k yes
samba-tool user create test_rename_user secretPWD123! --given-name=test --surname=rename --mail-address=test.user.rename@domain.alt --login-shell=/bin/bash -k yes
samba-tool user create test_search_user secretPWD123! --given-name=test --surname=search --mail-address=test.user.search@domain.alt --login-shell=/bin/bash -k yes 
samba-tool user create test_block_user secretPWD123! --given-name=test --surname=block --mail-address=test.user.block@domain.alt --login-shell=/bin/bash -k yes

samba-tool ou create "ou=test_delete_ou,dc=domain,dc=alt" --description="Test OU delete" -k yes
samba-tool ou create "ou=test_rename_ou,dc=domain,dc=alt" --description="Test OU rename" -k yes
samba-tool ou create "ou=test_mod_ou,dc=domain,dc=alt" --description="Test OU mod" -k yes

samba-tool group add test_delete_group -k yes
samba-tool group add test_rename_group -k yes
samba-tool group add test_mod_group -k yes

samba-tool computer create test_rename_computer --description="Test computer rename" -k yes
samba-tool computer create test_mod_computer --description="Test computer modification" -k yes
samba-tool computer create test_delete_computer --description="Test computer delete" -k yes
