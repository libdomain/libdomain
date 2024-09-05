#include "ldap_parsers_tests.h"
#include <ldap_parsers.h>
#include <ldap_schema.h>
#include <common.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static const char* VALID_VALUES[] =
{
    "objectClasses: ( 1.2.840.113556.1.5.10 NAME 'classRegistration' SUP leaf STRUCTURAL MAY (requiredCategories $ managedBy $ implementedCategories $ cOMTreatAsClassId $ cOMProgID $ cOMOtherProgId $ cOMInterfaceID $ cOMCLSID ) ) objectClasses: ( 1.2.840.113556.1.5.84 NAME 'displaySpecifier' SUP top STRUCTURAL MAY (treatAsLeaf $ shellPropertyPages $ shellContextMenu $ scopeFlags $ queryFilter $ iconPath $ extraColumns $ creationWizard $ createWizardExt $ createDialog $ contextMenu $ classDisplayName $ attributeDisplayNames $ adminPropertyPages $ adminMultiselectPropertyPages $ adminContextMenu ) )",
    "objectClasses: ( 1.2.840.113556.1.5.68 NAME 'applicationSiteSettings' SUP top ABSTRACT MAY (notificationList $ applicationName ) )",
    "objectClasses: ( 1.2.840.113556.1.5.7000.53 NAME 'crossRefContainer' SUP top STRUCTURAL MAY (msDS-EnabledFeature $ msDS-SPNSuffixes $ uPNSuffixes $ msDS-UpdateScript $ msDS-ExecuteScriptPassword $ msDS-Behavior-Version ) )",
    "objectClasses: ( 1.2.840.113556.1.5.5 NAME 'samServer' SUP securityObject STRUCTURAL MAY (samDomainUpdates ) )",
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const char* INVALID_VALUES[] =
{
    NULL,
    "1231231",
    "abcd"
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(parse_object_class_description_returns_object_class_on_valid_values)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    if (!talloc_ctx)
    {
        fail_test("parse_object_class_description_returns_object_class_on_valid_values - Unable to create talloc_ctx!\n");
    }

    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        LDAPObjectClass *object_class = parse_object_class(talloc_ctx, VALID_VALUES[i]);

        if (!object_class)
        {
            ld_error("parse_object_class_description_returns_object_class_on_valid_values - failed case %s.\n", VALID_VALUES[i]);
        }

        assert_that(object_class, is_non_null);
    }

    talloc_free(talloc_ctx);
}

Ensure(parse_object_class_description_returns_null_on_invalid_values)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    if (!talloc_ctx)
    {
        fail_test("parse_object_class_description_returns_null_on_invalid_values - Unable to create talloc_ctx!\n");
    }

    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        LDAPObjectClass *object_class = parse_object_class(talloc_ctx, INVALID_VALUES[i]);

        if (object_class != NULL)
        {
            ld_error("parse_object_class_description_returns_null_on_invalid_values - failed case %s.\n", INVALID_VALUES[i]);
        }

        assert_that(object_class, is_null);
    }

    talloc_free(talloc_ctx);
}

TestSuite* object_class_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, parse_object_class_description_returns_object_class_on_valid_values);
    add_test(suite, parse_object_class_description_returns_null_on_invalid_values);
    return suite;
}
