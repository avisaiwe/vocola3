// NatLinkConnectorC.h

#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Windows::Forms;

// Python can easily call a function exported from a DLL.
// C# can't export functions, so this thin C++ file does the job.
// These functions call C# methods (easier to read) to do the work.

static wchar_t VocolaExecutionFolder[1000];

Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args)
{
    try
    {
        String^ shortAssemblyName = args->Name->Substring(0, args->Name->IndexOf(','));
        String^ fileName = Path::Combine(gcnew String(VocolaExecutionFolder), shortAssemblyName + gcnew String(L".dll"));
        //String^ projectDir = gcnew System::String(L"C:\\Users\\Rick\\Rick\\Technical\\Vocola3\\OpenSource\\GoogleCode 3.1\\Source\\VocolaCore\\bin\\Debug");
        //String^ fileName = Path::Combine(projectDir, shortAssemblyName + gcnew System::String(L".dll"));
        if (File::Exists(fileName))
        {
            Assembly^ result = Assembly::LoadFrom(fileName);
            return result;
        }
        else
            return (Assembly::GetExecutingAssembly()->FullName == args->Name)
            ? Assembly::GetExecutingAssembly()
            : nullptr;
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(ex->Message);
    }
}

void ReallyInitializeConnection()
{
    Vocola::NatLinkToVocolaClient::InitializeConnection();
}

static bool initialized = false;

extern "C" void __declspec(dllexport) __stdcall InitializeConnection(const wchar_t* vocolaExecutionFolder)
//extern "C" __declspec(dllexport) void InitializeConnection()
{
    try
    {
        if (initialized)
            return;
        wcscpy(VocolaExecutionFolder, vocolaExecutionFolder);

        // The executing AppDomain's "CodeBase" is the Python executable folder, so we have to specify how to find assemblies
        AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(CurrentDomain_AssemblyResolve);

        // If InitializeConnection() isn't called in a separate function the assembly load fails before the resolver is established
        ReallyInitializeConnection();
        initialized = true;
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(ex->Message);
    }
}

extern "C" void __declspec(dllexport) __stdcall RunActions(const wchar_t* commandId, const wchar_t* variableWords)
{
    Vocola::NatLinkToVocolaClient::RunActions(gcnew String(commandId), gcnew String(variableWords));
}
