--[[
Autores:
Bernard Jean Lupiac	1311792
Miguel Baldez Boing	1312013
]]--

local tossam = require("tossam") 

local exit = false
--Tabela com os valores dos sensores
local sensortable = {}
--Variáveis de estado para qual é a "leva" de medidas e quantos sensores ja foram lidos nela
local nosLidos = 0
local contador = 0

--Obtendo valores do config.txt:
local f = assert(io.open("config.txt", "r"))
--Quantidade total de nos na rede
local totalDeNos = f:read("*number")
f:read("*line")
--Qual sensor o usuario deseja obter
local sensorType = f:read(1)
f:close()

--As funcoes genOrderedIndex, orderedNext e orderedPairs são usadas para ordenar uma tabela pela sua chave
function __genOrderedIndex( t )
    local orderedIndex = {}
    for key in pairs(t) do
        table.insert( orderedIndex, key )
    end
    table.sort( orderedIndex )
    return orderedIndex
end

function orderedNext(t, state)
    -- Equivalent of the next function, but returns the keys in the alphabetic
    -- order. We use a temporary ordered key table that is stored in the
    -- table being iterated.

    key = nil
    --print("orderedNext: state = "..tostring(state) )
    if state == nil then
        -- the first time, generate the index
        t.__orderedIndex = __genOrderedIndex( t )
        key = t.__orderedIndex[1]
    else
        -- fetch the next value
        for i = 1,table.getn(t.__orderedIndex) do
            if t.__orderedIndex[i] == state then
                key = t.__orderedIndex[i+1]
            end
        end
    end

    if key then
        return key, t[key]
    end

    -- no more value to return, cleanup
    t.__orderedIndex = nil
    return
end

function orderedPairs(t)
    -- Equivalent of the pairs() function on tables. Allows to iterate
    -- in order
    return orderedNext, t, nil
end

while not(exit) do
	local mote = tossam.connect("sf@localhost:9002",1)
	--local mote = tossam.connect("serial@/dev/ttyUSB1:micaz",1)
	if not(mote) then print("Connection error!"); return(1); end


	mote:register [[ 
	  nx_struct msg_serial [145] { 
		nx_uint8_t id;
		nx_uint16_t source;
		nx_uint16_t target;	
		nx_uint8_t  d8[4]; 
		nx_uint16_t d16[4];
		nx_uint32_t d32[2];
	  }; 
	]]


	while (mote) do

		local stat, msg, emsg = pcall(function() return mote:receive() end) 
		if stat then
			if msg then
				--Verifica se a mensagem nao eh a inicial, de formacao da arvore
				if msg.d8[1] == 0 then
					--Obtem os valores da mensagem que chegou ao BaseStation
					local nodeId = msg.source
					local valor
					if sensorType == "t" then valor = msg.d16[2]
					elseif sensorType == "l" then valor = msg.d16[3]
					else
						print("Tipo de sensor inválido!")
						exit = true
						break 
					end

					--Insere o valor lido na tabela
					sensortable[nodeId] = valor
					nosLidos = nosLidos + 1

					--Se o valor lido for o ultimo da "leva"
					if nosLidos == totalDeNos then
						--Escrever um novo arquivo com os valores da tabela
						local batch = assert(io.open("Data/batch"..contador..".txt", "w"))
						for i,k in orderedPairs(sensortable) do
							batch:write(k..'\n')
						end
						batch:close()
						--Limpar tabela e ajeitar variáveis de estado
						sensortable = {}
						nosLidos = 0
						contador = contador + 1
					end
			
				end
			else
				if emsg == "closed" then
					print("\nConnection closed!")
					exit = true
					break 
				end
			end
		else
			print("\nreceive() got an error:"..msg)
			exit = true
			break
		end
	end

	mote:unregister()
	mote:close() 
end


