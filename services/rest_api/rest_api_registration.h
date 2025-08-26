#pragma once

#include "rest_api_service.h"
#include "../../framework/service_factory.h"

namespace ServiceFramework {

// Register the RestApiService with the factory
REGISTER_SERVICE(RestApiService, "RestApiService");

} // namespace ServiceFramework
