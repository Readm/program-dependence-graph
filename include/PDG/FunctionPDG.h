#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "PDGLLVMNode.h"

namespace pdg {
class FunctionPDG
{
public:
    using PDGNodeTy = std::shared_ptr<PDGNode>;
    using PDGLLVMArgumentNodes = std::unordered_map<llvm::Argument*, PDGNodeTy>;
    using PDGLLVMNodes = std::unordered_map<llvm::Value*, PDGNodeTy>;
    using PDGNodes = std::vector<PDGNode*>;
    using arg_iterator = PDGLLVMArgumentNodes::iterator;
    using arg_const_iterator = PDGLLVMArgumentNodes::const_iterator;
    using llvm_iterator = PDGLLVMNodes::iterator;
    using const_llvm_iterator = PDGLLVMNodes::const_iterator;
    using iterator = PDGNodes::iterator;
    using const_iterator = PDGNodes::const_iterator;
    using CallSites = std::vector<llvm::CallSite>;

public:
    explicit FunctionPDG(llvm::Function* F)
        : m_function(F)
        , m_functionDefinitionBuilt(false)
    {
        if (F->isVarArg()) {
            m_vaArgNode.reset(new PDGLLVMVaArgNode(F));
        }
    }

    ~FunctionPDG() = default;
    FunctionPDG(const FunctionPDG& ) = delete;
    FunctionPDG(FunctionPDG&& ) = delete;
    FunctionPDG& operator =(const FunctionPDG& ) = delete;
    FunctionPDG& operator =(FunctionPDG&& ) = delete;


public:
    llvm::Function* getFunction()
    {
        return m_function;
    }

    const llvm::Function* getFunction() const
    {
        return const_cast<FunctionPDG*>(this)->getFunction();
    }


    void setFunctionDefBuilt(bool built)
    {
        m_functionDefinitionBuilt = built;
    }
    bool isFunctionDefBuilt() const
    {
        return m_functionDefinitionBuilt;
    }

    bool isVarArg() const
    {
        return m_function->isVarArg();
    }

    PDGNodeTy getVaArgNode()
    {
        return m_vaArgNode;
    }

    const PDGNodeTy getVaArgNode() const
    {
        return m_vaArgNode;
    }

    bool hasFormalArgNode(llvm::Argument* arg) const
    {
        return m_formalArgNodes.find(arg) != m_formalArgNodes.end();
    }
    bool hasNode(llvm::Value* value) const
    {
        return m_functionLLVMNodes.find(value) != m_functionLLVMNodes.end();
    }

    PDGNodeTy getFormalArgNode(llvm::Argument* arg)
    {
        assert(hasFormalArgNode(arg));
        return m_formalArgNodes.find(arg)->second;
    }
    const PDGNodeTy getFormalArgNode(llvm::Argument* arg) const
    {
        return const_cast<FunctionPDG*>(this)->getFormalArgNode(arg);
    }

    PDGNodeTy getNode(llvm::Value* val)
    {
        assert(hasNode(val));
        return m_functionLLVMNodes.find(val)->second;
    }
    const PDGNodeTy getNode(llvm::Value* val) const
    {
        return const_cast<FunctionPDG*>(this)->getNode(val);
    }

    bool addFormalArgNode(llvm::Argument* arg, PDGNodeTy argNode)
    {
        auto res = m_formalArgNodes.insert(std::make_pair(arg, argNode));
        if (res.second) {
            m_functionNodes.push_back(res.first->second.get());
        }
        return res.second;
    }
    bool addFormalArgNode(llvm::Argument* arg)
    {
        if (hasFormalArgNode(arg)) {
            return false;
        }
        auto res = m_formalArgNodes.insert(std::make_pair(arg, PDGNodeTy(new PDGLLVMFormalArgumentNode(arg))));
        assert(res.second);
        m_functionNodes.push_back(res.first->second.get());
        return true;
    }

    bool addNode(llvm::Value* val, PDGNodeTy node)
    {
        auto res = m_functionLLVMNodes.insert(std::make_pair(val, node));
        if (res.second) {
            m_functionNodes.push_back(res.first->second.get());
        }
        return res.second;
    }

    bool addNode(PDGNodeTy node)
    {
        m_functionNodes.push_back(node.get());
    }

    void addCallSite(const llvm::CallSite& callSite)
    {
        m_callSites.push_back(callSite);
    }

    const CallSites& getCallSites() const
    {
        return m_callSites;
    }

public:
    arg_iterator formalArgBegin()
    {
        return m_formalArgNodes.begin();
    }
    arg_iterator formalArgEnd()
    {
        return m_formalArgNodes.end();
    }
    arg_const_iterator formalArgBegin() const
    {
        return m_formalArgNodes.begin();
    }
    arg_const_iterator formalArgEnd() const
    {
        return m_formalArgNodes.end();
    }

    llvm_iterator llvmNodesBegin()
    {
        return m_functionLLVMNodes.begin();
    }
    llvm_iterator llvmNodesEnd()
    {
        return m_functionLLVMNodes.end();
    }
    const_llvm_iterator llvmNodesBegin() const
    {
        return m_functionLLVMNodes.begin();
    }
    const_llvm_iterator llvmNodesEnd() const
    {
        return m_functionLLVMNodes.end();
    }

    iterator nodesBegin()
    {
        return m_functionNodes.begin();
    }
    iterator nodesEnd()
    {
        return m_functionNodes.end();
    }
    const_iterator nodesBegin() const
    {
        return m_functionNodes.begin();
    }
    const_iterator nodesEnd() const
    {
        return m_functionNodes.end();
    }

    unsigned size() const
    {
        return m_functionNodes.size();
    }

    CallSites::iterator callSitesBegin()
    {
        return m_callSites.begin();
    }

    CallSites::const_iterator callSitesBegin() const
    {
        return m_callSites.begin();
    }

    CallSites::iterator callSitesEnd()
    {
        return m_callSites.end();
    }

    CallSites::const_iterator callSitesEnd() const
    {
        return m_callSites.end();
    }

    const std::string getGraphName() const
    {
        return m_function->getName();
    }

private:
    llvm::Function* m_function;
    bool m_functionDefinitionBuilt;
    PDGLLVMArgumentNodes m_formalArgNodes;
    PDGNodeTy m_vaArgNode;
    // TODO: formal ins, formal outs? formal vaargs?
    PDGLLVMNodes m_functionLLVMNodes;
    PDGNodes m_functionNodes;
    CallSites m_callSites;
}; // class FunctionPDG

} // namespace pdg

