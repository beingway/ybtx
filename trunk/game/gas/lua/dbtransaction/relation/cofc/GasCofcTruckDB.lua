--
--local CCofcTruck = class()
--
--local CofcTruckSql = CreateDbBox(...)
----------------------------------------------------------------------------------------------------------
--local StmtDef = {
--		"SaveCofcTruckInfo",
--		[[
--			insert into tbl_cofc_truck(ct_uCarTyp,ct_uCapacity,ct_uHP,cs_uId) values(?,?,?,?)
--		]]
--}
--DefineSql(StmtDef,CCofcTruck)
--
----@brief 保存运输陈信息
----@param carTyp:车的类型
----@param capacity:容量
----@param car_HP:生命值
--function CofcTruckSql.SaveCofcTruckInfo(Conn,carTyp,capacity,car_HP)
--	local playerId = data["playerId"]
--	local carTyp = data["carTyp"]
--	local capacity = data["capacity"]
--	local car_HP = data["car_HP"]
--	CCofcTruck.SaveCofcTruckInfo:ExecSql("",carTyp,capacity,car_HP)
--	
--	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
--		CancelTran()
--		return nil
--	else
--		return g_DbChannelMgr:LastInsertId()
--	end
--end
------------------------------------------------------------------------------------------------------------
--local StmtDef = {
--		"GetCofcTruckInfo",
--		[[
--			select 
--				ct_uCarTyp,ct_uCapacity,ct_uHP
--			from 
--				tbl_cofc_truck
--			where
--				ct_uId = ? and cs_uId = ?
--		]]
--}
--DefineSql(StmtDef,CCofcTruck)
--
----@brief 查询运输车信息
----@param carId:车的编号
--function CofcTruckSql.GetCofcTruckInfo(data)
--	local playerId = data["playerId"]
--	local carId = data["carId"]
--	local cofcTruckTbl = {}
--	local cofcTruckInfo = CCofcTruck.GetCofcTruckInfo("nnn",carId,playerId)
--	if nil == cofcTruckInfo or cofcTruckInfo:GetRowNum() == 0 then
--		cofcTruckInfo:Release()
--		return cofcTruckTbl
--	end
--	cofcTruckTbl[1] = cofcTruckInfo:GetData(0,0)
--	cofcTruckTbl[2] = cofcTruckInfo:GetData(0,1)
--	cofcTruckTbl[3] = cofcTruckInfo:GetData(0,2)
--	cofcTruckInfo:Release()
--	return cofcTruckTbl
--end
--
------------------------------------------------------------------------------------------------------------
--return CofcTruckSql