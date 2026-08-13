#pragma once

namespace InternalSkins
{
    // Resolves the econ schema offsets once. Returns false if the schema
    // system did not give us the fields we need.
    bool ResolveOffsets();

    // Applies the fixed SkinTable to every weapon the local player carries.
    // Called every tick from the internal worker thread.
    void Run();
}
