/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef mitkRESTManager_h
#define mitkRESTManager_h

#include <QObject>
#include <QThread>

#include <mitkIRESTManager.h>
#include <mitkIRESTObserver.h>
#include <mitkRESTClientMicroService.h>
#include <mitkRESTServerMicroService.h>

#include <MitkCppRestSdkExports.h>

namespace mitk
{
  class MITKCPPRESTSDK_EXPORT RESTManager : public QObject, public IRESTManager
  {
     Q_OBJECT

  public:
    RESTManager();
    ~RESTManager() override;

    /**
     * @brief Executes a HTTP request in the mitkRESTClientMicroService class
     *
     * @param uri defines the URI the request is send to
     * @param type the RequestType of the HTTP request (optional)
     * @param body the body for the request (optional)
     * @return task to wait for
     */
    pplx::task<web::json::value> SendRequest(web::uri uri,
                                             RequestType type = get,
                                             web::json::value = NULL,
                                             utility::string_t filePath = L"") override;
    
    /**
     * @brief starts listening for requests if there isn't another observer listening and the port is free
     *
     * @param uri defines the URI for which incoming requests should be send to the observer
     * @param observer the observer which handles the incoming requests
     */
    void ReceiveRequest(web::uri uri, IRESTObserver *observer) override;

    /**
     * @brief Handles incoming requests by notifying the observer which should receive it
     *
     * @param uri defines the URI of the request
     * @param body the body of the request
     * @return the data which is modified by the notified observer
     */
    web::json::value Handle(web::uri uri, web::json::value body) override;

    /**
     * @brief Handles the deletion of an observer for all or a specific uri
     *
     * @param observer the observer which shouldn't receive requests anymore
     * @param uri the uri for which the observer doesn't handle requests anymore (optional)
     */
    virtual void HandleDeleteObserver(IRESTObserver *observer, web::uri uri) override;

  private:
    std::map<int, RESTClientMicroService *> m_ClientMap;                     // Map with port client pairs
    std::map<int, RESTServerMicroService *> m_ServerMap;                     // Map with port server pairs
    std::map<int, QThread*> m_ServerThreadMap;                               // Map with threads for servers
    std::map<std::pair<int, utility::string_t>, IRESTObserver *> m_Observer; // Map with all observers
  };
} // namespace mitk
#endif // !mitkRESTManager_h
