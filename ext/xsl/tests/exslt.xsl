<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:date="http://exslt.org/dates-and-times"
                extension-element-prefixes="date">

<xsl:output method="text"/>

<xsl:template match="date">
  Test Date : <xsl:value-of select="@date"/>
    <!-- dateTime, date, gYearMonth or gYear; else NaN -->
    year                 : <xsl:value-of select="date:year(@date)"/>
    <!-- dateTime, date, gYearMonth or gYear; else NaN -->
    leap-year            : <xsl:value-of select="date:leap-year(@date)"/>
    <!-- dateTime, date, gYearMonth, gMonth or gMonthDay; else NaN -->
    month-in-year        : <xsl:value-of select="date:month-in-year(@date)"/>
    <!-- dateTime, date, gYearMonth or gMonth; else '' -->
    month-name           : <xsl:value-of select="date:month-name(@date)"/>
    <!-- dateTime, date, gYearMonth or gMonth; else '' -->
    month-abbreviation   : <xsl:value-of select="date:month-abbreviation(@date)"/>
    <!-- dateTime or date; else NaN -->
    week-in-year         : <xsl:value-of select="date:week-in-year(@date)"/>
    <!-- dateTime, date; else NaN -->
    day-in-year          : <xsl:value-of select="date:day-in-year(@date)"/>
    <!-- dateTime, date, gMonthDay or gDay; else NaN -->
    day-in-month         : <xsl:value-of select="date:day-in-month(@date)"/>
    <!-- dateTime, date; else NaN -->
    day-of-week-in-month : <xsl:value-of select="date:day-of-week-in-month(@date)"/>
    <!-- dateTime, date; else NaN -->
    day-in-week          : <xsl:value-of select="date:day-in-week(@date)"/>
    <!-- dateTime or date; else NaN -->
    day-name             : <xsl:value-of select="date:day-name(@date)"/>
    <!-- dateTime or date; else NaN -->
    day-abbreviation     : <xsl:value-of select="date:day-abbreviation(@date)"/>
    <!-- dateTime or time;  else '' -->
    time                 : <xsl:value-of select="date:time(@date)"/>
    <!-- dateTime or time;  else NaN -->
    hour-in-day          : <xsl:value-of select="date:hour-in-day(@date)"/>
    <!-- dateTime or time;  else NaN -->
    minute-in-hour       : <xsl:value-of select="date:minute-in-hour(@date)"/>
    <!-- dateTime or time;  else NaN -->
    second-in-minute     : <xsl:value-of select="date:second-in-minute(@date)"/>
</xsl:template>

</xsl:stylesheet>

