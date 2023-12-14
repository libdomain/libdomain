# Import the required module
Import-Module -Name "ActiveDirectory"

# Create users
New-ADUser -Name "test delete" -GivenName "test" -Surname "delete" -UserPrincipalName "test.user.delete@domain.alt" -SamAccountName "test_delete_user" -Enabled $true -AccountPassword (ConvertTo-SecureString -AsPlainText "secretPWD123!" -Force) -PassThru
New-ADUser -Name "test mod" -GivenName "test" -Surname "mod" -UserPrincipalName "test.user.mod@domain.alt" -SamAccountName "test_mod_user" -Enabled $true -AccountPassword (ConvertTo-SecureString -AsPlainText "secretPWD123!" -Force) -PassThru
New-ADUser -Name "test rename" -GivenName "test" -Surname "rename" -UserPrincipalName "test.user.rename@domain.alt" -SamAccountName "test_rename_user" -Enabled $true -AccountPassword (ConvertTo-SecureString -AsPlainText "secretPWD123!" -Force) -PassThru
New-ADUser -Name "test search" -GivenName "test" -Surname "search" -UserPrincipalName "test.user.search@domain.alt" -SamAccountName "test_search_user" -Enabled $true -AccountPassword (ConvertTo-SecureString -AsPlainText "secretPWD123!" -Force) -PassThru
New-ADUser -Name "test block" -GivenName "test" -Surname "block" -UserPrincipalName "test.user.block@domain.alt" -SamAccountName "test_block_user" -Enabled $true -AccountPassword (ConvertTo-SecureString -AsPlainText "secretPWD123!" -Force) -PassThru
New-ADUser -Name "Manager" -GivenName "Manager" -Surname "Manager" -UserPrincipalName "Manager@domain.alt" -SamAccountName "Manager" -Enabled $true -AccountPassword (ConvertTo-SecureString -AsPlainText "secretPWD123!" -Force) -PassThru

# Create OUs
New-ADOrganizationalUnit -Name "test_delete_ou" -Description "Test OU delete" -PassThru
New-ADOrganizationalUnit -Name "test_rename_ou" -Description "Test OU rename" -PassThru
New-ADOrganizationalUnit -Name "test_mod_ou" -Description "Test OU mod" -PassThru

# Create groups
New-ADGroup -Name "test_delete_group" -GroupScope Global -PassThru
New-ADGroup -Name "test_rename_group" -GroupScope Global -PassThru
New-ADGroup -Name "test_mod_group" -GroupScope Global -PassThru

# Create computers
New-ADComputer -Name "test_rename_c" -Description "Test computer rename" -PassThru
New-ADComputer -Name "test_mod_c" -Description "Test computer modification" -PassThru
New-ADComputer -Name "test_delete_c" -Description "Test computer delete" -PassThru
