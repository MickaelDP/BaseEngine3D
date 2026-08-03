#pragma once

// Step 6: the contract every engine subsystem must honour.
//
// A subsystem is any self-contained piece of engine functionality with a
// lifecycle: rendering, physics, audio, procedural generation, asset
// baking. The Engine owns them and drives all three phases in a
// guaranteed order, without knowing what any of them actually does.
//
// This is what makes the engine extensible: adding a
// ProceduralGenSubsystem later requires zero changes to Engine.cpp.
class ISubsystem {
    public:
        // Virtual destructor is mandatory here. The Engine holds these as
        // ISubsystem pointers; without `virtual`, deleting through the
        // base pointer would skip the derived destructor and leak every
        // GL handle the subsystem owns.
        virtual ~ISubsystem() = default;

        // Called once, in registration order, before the loop starts.
        // GL context is already active at this point.
        virtual void init() = 0;

        // Called once per frame. dt is in seconds.
        virtual void update(float dt) = 0;

        // Called once, in REVERSE registration order, after the loop ends.
        virtual void shutdown() = 0;

        // Human-readable name, used in startup/shutdown logs. Makes an
        // init failure immediately traceable to a specific subsystem.
        virtual const char* getName() const = 0;
};