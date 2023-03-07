/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.s

============================================================================*/

#ifndef QmitkSetupVirtualEnvUtil_h_Included
#define QmitkSetupVirtualEnvUtil_h_Included

#include "mitkLogMacros.h"
#include "mitkProcessExecutor.h"
#include <MitkSegmentationUIExports.h>
#include <QString>

/**
 * @brief Class to ...
 */
class MITKSEGMENTATIONUI_EXPORT QmitkSetupVirtualEnvUtil
{
public:
  enum Tool
  {
    TOTALSEGMENTATOR
  };

  QmitkSetupVirtualEnvUtil(const QString baseDir);
  QmitkSetupVirtualEnvUtil();

  bool SetupVirtualEnv(Tool packageName = Tool::TOTALSEGMENTATOR);
  void PipInstall(const std::string &library,
                  const std::string &workingDir,
                  void (*callback)(itk::Object *, const itk::EventObject &, void *),
                  const std::string &command = "pip3");
  void ExecutePython(const std::string &args,
                     const std::string &pythonPath,
                     void (*callback)(itk::Object *, const itk::EventObject &, void *),
                     const std::string &command = "python");

  void InstallPytorch(const std::string &workingDir, void (*callback)(itk::Object *, const itk::EventObject &, void *));

  std::map<std::string, std::string> GetInstallParameters(QmitkSetupVirtualEnvUtil::Tool);
  QString GetBaseDir();
  QString GetVirtualEnvPath();
  void SetSystemPythonPath(QString& path);
  bool IsPythonPath(const QString &pythonPath);
  static void PrintProcessEvent(itk::Object * /*pCaller*/, const itk::EventObject &e, void *);

private:
  QString m_BaseDir;
  QString m_venvPath;
  QString m_SysPythonPath;
};

#endif