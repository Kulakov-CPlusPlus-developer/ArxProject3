# ARXProject

## This is a simple collision detection plugin.

My autocad plugin allows you to find all the lines in a drawing and check their collinearity with respect to each other.

## Функциональные возможности

- The `GetAllLines()` function finds all the lines in the drawing and fills in the `AcDbObjectIdArray lineIds` array.
- The `SearchForOverlays()` function checks the collinearity of all lines. In the case of collinearity, a notification dialog box is displayed, and a hint appears at the collinearity place.

## Установка

1. Склонируйте репозиторий:
   ```bash
   git clone https://github.com/ваш_пользователь/ваш_репозиторий.git
   ```
2. Open the file acrxEntryPoint.cpp in Visual Studio 2022.
3. Build the project. The result should be the file \ArxProject3\x64\Debug\ADSKArxProject3.arx.
4. Open AutoCAD, run the APPLOAD command, specify the path to your \ArkProject3\x64\Debug\ADSKArxProject3.arx and download it.
5. Run the STARTTEST command to run the check. 

## Attention!
Do not run the STARTTEST command again until you have eliminated all the collinearity points in the drawing. This can lead to new collisions.

## System requirements
1. ObjectARXWizard2024
2. Visual Studio 2022
3. MFC(latest for build version)
4. ObjectARX SDK 2024
