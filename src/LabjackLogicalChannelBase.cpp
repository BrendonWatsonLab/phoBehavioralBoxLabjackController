#include "LabjackLogicalChannelBase.h"

LabjackLogicalChannelBase::LabjackLogicalChannelBase(std::vector<std::string> port_names,
                                             const std::vector<std::string>& port_purpose, const std::string& name,
                                             Direction direction): portNames(std::move(port_names)),
                                                                   portPurpose(port_purpose),
                                                                   name(name),
                                                                   direction(direction)
{
}
