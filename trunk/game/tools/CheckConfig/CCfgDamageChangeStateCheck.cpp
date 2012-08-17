#include "stdafx.h"
#include "CCfgAllStateCheck.h"
#include "CCfgColChecker.inl"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CSkillCfg.h"

CCfgDamageChangeStateCheck::SetRowName CCfgDamageChangeStateCheck::ms_setRowName;
CSkillCfg::MapTempVarType			CSkillCfg::ms_mapTempVarType;
CSkillCfg::MapPassivityTempVarType	CSkillCfg::ms_mapPassivityTempVarType;

bool CCfgDamageChangeStateCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TxtTableFile;
	SetTabFile(TxtTableFile, "伤害变更状态");
	CSkillCfg::InitDCStateTempVarMap();
	CSkillCfg::InitPassivityDCStateTempVarMap();
	if (!TxtTableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 18;
	if (TxtTableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TxtTableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for(int32 i = 1; i < TxtTableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string strName,	strCancelEff, strTriggerEvent;
		bool bPersistent;
		CCfgCalc* pCalcTime = new CCfgCalc;
		ReadItem(strName,		szTriggerState_Name,		CANEMPTY);
		ReadItem(bPersistent,	szState_Persistent,			CANEMPTY,	NO);
		ReadItem(pCalcTime,		szDamageChangeState_Time,			GE,			-1);
		ReadItem(strTriggerEvent, szDamageChangeState_TriggerEvent, CANEMPTY);
		ReadItem(strCancelEff,	szTriggerState_CancelableMagicEff,	CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strCancelEff);
		trimend(strName);
		SetRowName::iterator iter = ms_setRowName.find(strName);
		if (iter != ms_setRowName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagicState_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setRowName.insert(strName);
			CCfgAllStateCheck* pState = new CCfgAllStateCheck;
			pState->m_strName = strName;
			pState->m_eType = eBST_Damage;
			pState->m_bPersistent = bPersistent;
			pState->m_bNeedSaveToDB =
				pCalcTime->IsSingleNumber() && pCalcTime->GetDblValue() > 10
				&& (strTriggerEvent != "被安装者杀死" && strTriggerEvent != "安装者死亡") ? true : false;
			if (!strCancelEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strCancelEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strCancelEff);
				pState->m_pCancelEff = (*itr).second;
			}
			CCfgAllStateCheck::ms_mapState.insert(make_pair(strName, pState));
		}

		bool bArg2;
		ReadItem(bArg2,		szDamageChangeState_Passivity, CANEMPTY, NO);
		string sTempVar;
		if (bArg2)
			sTempVar = CSkillCfg::ms_mapPassivityTempVarType[strTriggerEvent];
		else
			sTempVar = CSkillCfg::ms_mapTempVarType[strTriggerEvent];
		if (sTempVar == "")
		{
			stringstream sExp;
			sExp << "\n第[" << i << "]行违反表约束：伤害变更暂时不支持的触发事件[" << strTriggerEvent << "]";
			GenExpInfo(sExp.str());
		}

		CCfgCalc* pCalc = NULL;
		ReadMixedItem(pCalc, szTplState_Scale, CANEMPTY, "");
		if (!pCalc->GetTestString().empty() && pCalc->GetValueCount() > 2)
		{
			GenExpInfo("表达式个数大于2", pCalc->GetTestString());
		}
		delete pCalc;

		string strSkillType, strAttackType;
		ReadItem(strSkillType, szDamageChangeState_SkillType, CANEMPTY);
		ReadItem(strAttackType, szDamageChangeState_AttackType, CANEMPTY);
		if(strSkillType.empty() && !strAttackType.empty())
		{
			GenExpInfo("违反表约束：技能类型为空时不能填攻击类型", strAttackType);
		}

		string strFinalEff;
		ReadItem(strFinalEff,	szDamageChangeState_FinalMagicEff,	CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strFinalEff);
		if(!strCancelEff.empty() && !strFinalEff.empty() && strCancelEff == strFinalEff)
		{
			stringstream str;
			str << "\n第" << i << "行违反表约束：同一行的不同魔法效果不能同名";
			GenExpInfo(str.str());
		}

		// 特效名检查
		string strFX = TxtTableFile.GetString(i, szTplState_FXID);
		if (strFX != "")
		{
			vector<string> sFXTable = CCfgMagicOp::Split(strFX, ",");
			if (sFXTable.size() == 2 || sFXTable.size() == 3)
			{
				string strFXName	= sFXTable[1];
				if (strFXName == "")
				{
					stringstream ExpStr;
					ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 逗号后必须有特效名!";
					GenExpInfo(ExpStr.str());
				}
			}
			else
			{
				stringstream ExpStr;
				ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 必须为逗号隔开的两项!";
				GenExpInfo(ExpStr.str());
			}
		}
	}
	return true;
}

void CCfgDamageChangeStateCheck::EndCheck()
{
	ms_setRowName.clear();
}

bool CCfgDamageChangeStateCheck::CheckExist(const string& strName)
{
	SetRowName::iterator iter = ms_setRowName.find(strName);
	if (iter == ms_setRowName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 伤害变更状态" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
