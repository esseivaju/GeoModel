#include "Celeritas.hh"

#include <memory>
#include <G4Threading.hh>
#include <G4Version.hh>
#include <accel/AlongStepFactory.hh>
#include <accel/LocalTransporter.hh>
#include <accel/SetupOptions.hh>
#include <accel/SetupOptionsMessenger.hh>
#include <accel/SharedParams.hh>
#include <celeritas/field/UniformFieldData.hh>
#include <celeritas/io/ImportData.hh>

using namespace celeritas;

// Global shared setup options
SetupOptions& CelerSetupOptions()
{
    static SetupOptions options = [] {
        // Construct setup options the first time CelerSetupOptions is invoked
        // Ideally, we should also set up SetupOptionsMessenger so these can be
        // adjusted
        SetupOptions so;

        // Set along-step factory
        so.make_along_step = celeritas::UniformAlongStepFactory();

        so.action_times = false;

        so.max_num_tracks = 65536;
        so.max_num_events = 10000;
        so.initializer_capacity = so.max_num_tracks * 128;
        so.secondary_stack_factor = 3.0;
        so.ignore_processes = {"CoulombScat"};
        if constexpr (G4VERSION_NUMBER >= 1110)
        {
            so.ignore_processes.push_back("Rayl");
        }

        // Use Celeritas "hit processor" to call back to Geant4 SDs.
        // Not set just yet as we don't have these yet for TileCal etc
        so.sd.enabled = false;

        // Only call back for nonzero energy depositions: this is currently a
        // global option for all detectors, so if any SDs extract data from
        // tracks with no local energy deposition over the step, it must be set
        // to false.
        so.sd.ignore_zero_deposition = true;

        // Using the pre-step point, reconstruct the G4 touchable handle.
        so.sd.locate_touchable = true;

        // Pre-step time is used
        so.sd.pre.global_time = true;

        // Save diagnostic information
        so.output_file = "minceleritas.json";

        // Sort tracks
        so.track_order = TrackOrder::unsorted;

        return so;
    }();

    static auto mess = std::make_unique<SetupOptionsMessenger>(&options);

    return options;
}

// Shared data and GPU setup
SharedParams& CelerSharedParams()
{
    static SharedParams sp;
    return sp;
}

// Thread-local transporter
LocalTransporter& CelerLocalTransporter()
{
    static G4ThreadLocal LocalTransporter lt;
    return lt;
}

// Thread-local offload interface
SimpleOffload& CelerSimpleOffload()
{
    static G4ThreadLocal SimpleOffload so;
    return so;
}
