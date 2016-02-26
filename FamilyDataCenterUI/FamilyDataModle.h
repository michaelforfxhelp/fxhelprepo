#pragma once
#include <string>
#include <vector>

#include "FamilyDataCenterUI/Application.h"
#include "FamilyDataCenterUI/Range.h"
#include "FamilyDataCenterUI/Workbook.h"
#include "FamilyDataCenterUI/Workbooks.h"
#include "FamilyDataCenterUI/Worksheet.h"
#include "FamilyDataCenterUI/Worksheets.h"

class FamilyDataModle
{
public:
    FamilyDataModle();
    ~FamilyDataModle();
};


class ExcelHelper
{
public:
    ExcelHelper();
    ~ExcelHelper();

    bool Init();
    bool Create(const std::wstring& filepath);
    bool Open(const std::wstring& filepath);
    bool Close();

    bool ExportToExcel(std::vector<std::vector<std::wstring>> data);

private:

    bool OpenSheet(const std::wstring& sheetname, CWorksheet* sheet);

    CApplication app_;
    CWorkbook book_;
    CWorkbooks books_;
    CWorksheets sheets_;
};
