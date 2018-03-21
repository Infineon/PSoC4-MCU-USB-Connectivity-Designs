-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-046 kit.
-- =============================================================================

-- === RGB LED ===
attribute port_location of LED_RED(0)   : label is "PORT(5,2)";
attribute port_location of LED_GREEN(0) : label is "PORT(5,3)";
attribute port_location of LED_BLUE(0)  : label is "PORT(5,4)";