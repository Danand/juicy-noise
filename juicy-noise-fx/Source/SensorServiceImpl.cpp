#include "Generated/Proto/sensor-service.pb.h"
#include "Generated/Proto/sensor-service.grpc.pb.h"

class SensorServiceImpl final : public JuicyNoise::SensorsService::Service
{
  ::grpc::Status send(
    ::grpc::ServerContext* context,
    ::grpc::ServerReader<::JuicyNoise::Sensors>* reader,
    ::JuicyNoise::Empty* response)
    override
  {
    return ::grpc::Status();
  }
};