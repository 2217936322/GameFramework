local Scene = require('script.lib.core.scene')
local Gradient = require('script.lib.ui.gradient')
local AbsoluteLayout = require('script.lib.ui.layout.abs')
local TableLayout = require('script.lib.ui.layout.table')
local LinearLayout = require('script.lib.ui.layout.linear')
local Empty = require('script.lib.ui.empty')
local Block = require('script.lib.ui.block')
local Text = require('script.lib.ui.text')
local Button = require('script.lib.ui.comctl.button')
local Edit = require('script.lib.ui.comctl.edit')
local Radius = require('script.lib.ui.radius')

local modname = 'Game2048Scene'
local M = Scene:new()
_G[modname] = M
package.loaded[modname] = M

function M:new(o)
	o = o or {}
	o.name = '2048 - Game Scene'
	o.state = {focused=nil, hover=nil}
	o.gamedef = {
		size = 4,
		display = {
			[0] = '',
			[1] = '2',
			[2] = '4',
			[3] = '8',
			[4] = '16',
			[5] = '32',
			[6] = '64',
			[7] = '128',
			[8] = '256',
			[9] = '512',
			[10] = '1024',
			[11] = '2048'
		},
		bgcolor = '#BBADA0',
		color = {
			[0] = '#776E65',
			[1] = '#776E65',
			[2] = '#776E65',
			[3] = '#F9F6F2',
			[4] = '#F9F6F2',
			[5] = '#F9F6F2',
			[6] = '#F9F6F2',
			[7] = '#F9F6F2',
			[8] = '#F9F6F2',
			[9] = '#F9F6F2',
			[10] = '#F9F6F2',
			[11] = '#F9F6F2'
		},
		bg = {
			[0] = '#CDC1B4',
			[1] = '#EEE4DA',
			[2] = '#EDE0C8',
			[3] = '#F2B179',
			[4] = '#F59563',
			[5] = '#F67C5F',
			[6] = '#F65E3B',
			[7] = '#EDCF72',
			[8] = '#EDCC61',
			[9] = '#EDC850',
			[10] = '#EDC53F',
			[11] = '#EDC22E'
		},
		fontsize = {
			[0] = '48',
			[1] = '48',
			[2] = '48',
			[3] = '48',
			[4] = '40',
			[5] = '40',
			[6] = '40',
			[7] = '32',
			[8] = '32',
			[9] = '32',
			[10] = '24',
			[11] = '24'
		},
		map = {},
		merged = {},
		ui = o,
		startnum = 2,
		score = 0,
		gamestate_type = {
			ready = 0,
			failed = 1,
			win = 2
		},
		gamestate = 0,
		direction_type = {
			none = 0,
			up = 1,
			right = 2,
			down = 3,
			left = 4,
		},
		direction = 0
	}
	setmetatable(o, self)
	self.__index = self
	return o;
end

function M:init()
	self.minw = 800
	self.minh = 600
	UIExt.set_minw(self.minw, self.minh)

	UIExt.trace('Scene [2048 Game Page] init')
	-- INFO
	local info = UIExt.info()
	-- BG
	local bg = AbsoluteLayout:new({
		right = info.width,
		bottom = info.height
	})
	self.layers.bg = self:add(bg)
	bg:add(Block:new({
		color = '#EEEEEE',
		right = info.width,
		bottom = info.height
	}))
	UIExt.trace('Scene [2048 Game Page]: create background #' .. self.layers.bg.handle)
	-- TEXT
	local cc = Text:new({
		color = '#222222',
		text = 'Made by bajdcc',
		size = 24,
		pre_resize = function(this, left, top, right, bottom)
			return right - 200, bottom - 50, right, bottom
		end,
		hit = function(this, evt)
			if evt == WinEvent.leftbuttondown then
				FlipScene('Button')
			end
		end
	})
	self.layers.cc = self:add(cc)
	UIExt.trace('Scene [2048 Game Page]: create text #' .. self.layers.cc.handle)
	-- TEXT
	local text = Text:new({
		color = '#222222',
		text = '2048',
		pre_resize = function(this, left, top, right, bottom)
			return left, top, right, bottom / 2
		end
	})
	self.layers.text = self:add(text)
	UIExt.trace('Scene [2048 Game Page]: create text #' .. self.layers.text.handle)
	-- MENU
	self:init_menu(info)

	-- EVENT
	self:init_event()

	self.resize(self)
	UIExt.paint()
end

function M:destroy()
	UIExt.trace('Scene [2048 Game Page] destroy')
	UIExt.clear_scene()
end

function M:init_event()
	self.handler[self.win_event.created] = function(this)
		UIExt.trace('Scene [2048 Game Page] Test created message!')
	end
	self.handler[self.win_event.timer] = function(this, id)
	end
	self.handler[self.win_event.keydown] = function(this, code, flags)
		local state = this.gamedef
		if code == SysKey.left then state.direction = state.direction_type.left
		elseif code == SysKey.up then state.direction = state.direction_type.up
		elseif code == SysKey.down then state.direction = state.direction_type.down
		elseif code == SysKey.right then state.direction = state.direction_type.right
		else return end
		game_move(state)
	end
end

function M:init_menu(info)
	game_init(self.gamedef)
	game_restart(self.gamedef)
end

-- GAME LOGIC
function game_init(state)
	math.randomseed(os.time())
	math.random(1,10000)
	local row = 4
	local col = 4
	local bg = LinearLayout:new({
		pre_resize = function(this, left, top, right, bottom)
			local w = left + (right - left) / 2
			local h = top + (bottom - top) / 2
			return w - 150, h - 50, w + 150, h + 250
		end
	})
	state.ui:add(bg)
	bg:add(Radius:new({
		color = state.bgcolor,
		radius = 5
	}))
	local menu = TableLayout:new({
		row = row,
		col = col,
		padleft = 5,
		padtop = 5,
		padright = 5,
		padbottom = 5
	})
	bg:add(menu)
	state.ui.layers.menu = menu
	for i=1,state.size*state.size do
		state.map[i] = 0
		state.merged[i] = false
		local tile = LinearLayout:new({
				padleft = 5,
				padtop = 5,
				padright = 5,
				padbottom = 5
		})
		tile.x = (i - 1) / state.size + 1
		tile.y = (i - 1) % state.size + 1
		tile:attach(state.ui.layers.menu)
		Radius:new({
			color = state.bg[0],
			radius = 4
		}):attach(tile)
		Text:new({
			color = state.color[0],
			text = state.display[0],
			size = 40,
			bold = 1
		}):attach(tile)
	end
	local score = Text:new({
		color = '#222222',
		text = '',
		size = 24,
		pre_resize = function(this, left, top, right, bottom)
			return left, bottom - 50, left + 200, bottom
		end
	})
	state.ui.layers.score = state.ui:add(score)
	local slider = LinearLayout:new({
		align = 2,
		pre_resize = function(this, left, top, right, bottom)
			local height = (bottom - top) / 2
			return left + 10, height - 100, left + 200, height + 100
		end
	})
	state.ui:add(slider)
	Button:new({
		text = '���¿�ʼ',
		size = 30,
		click = function()
			game_restart(CurrentScene.gamedef)
		end
	}):attach(slider)
	Text:new({
		text = '',
		size = 30,
		click = function()
			game_restart(CurrentScene.gamedef)
		end
	}):attach(slider)
	state.ui.layers.rtstatus = slider.children[2]
end

function game_paint(state)
	state.ui.layers.score.text = 'Score: ' .. state.score
	state.ui.layers.score:update()
	if state.gamestate == state.gamestate_type.ready then
		state.ui.layers.rtstatus.text = '��Ϸ��'
	elseif state.gamestate == state.gamestate_type.failed then
		state.ui.layers.rtstatus.text = '������'
	elseif state.gamestate == state.gamestate_type.win then
		state.ui.layers.rtstatus.text = '��Ӯ��'
	end
	state.ui.layers.rtstatus:update();
	for i=1,state.size*state.size do
		local k = state.map[i]
		local obj = state.ui.layers.menu.children[i]
		obj.children[1].color = state.bg[k]
		obj.children[1]:update()
		obj.children[2].color = state.color[k]
		obj.children[2].text = state.display[k]
		obj.children[2].size = state.fontsize[k]
		obj.children[2]:update()
	end
	UIExt.paint()
end

function game_restart(state)
	for i=1,state.size*state.size do
		state.map[i] = 0
		state.merged[i] = false
	end
	state.score = 0
	state.gamestate = state.gamestate_type.ready
	state.direction = state.direction_type.none
	for i=1,state.startnum do
		game_add_tile(state)
	end
	game_paint(state);
end

function game_add_tile(state)
	local newi
	if math.random(1,10) == 1 then newi = 4 else newi = 2 end
	local newk = game_get_random_available_cell(state)
	state.map[newk] = newi
end

function game_get_available_cells(state)
	local cells = {}
	for k,v in ipairs(state.map) do
		if v == 0 then
			cells[#cells + 1] = k
		end
	end
	return cells
end

function game_get_random_available_cell(state)
	local cells = game_get_available_cells(state)
	local r = math.random(1, #cells)
	return cells[r]
end

function game_get_direction_vector(direction)
	local map = {
		[1] = {x=-1, y=0}, --UP
		[2] = {x=0, y=1}, --RIGHT
		[3] = {x=1, y=0}, --DOWN
		[4] = {x=0, y=-1} --LEFT
	}
	return map[direction]
end

function game_build_traversals(state, vector)
	local traversals = {x={}, y={}}
	for i=1,state.size do
		if vector.x ~= 1 then
			traversals.x[#traversals.x + 1] = i
		else
			traversals.x[#traversals.x + 1] = state.size-i+1
		end
		if vector.y ~= 1 then
			traversals.y[#traversals.y + 1] = i
		else
			traversals.y[#traversals.y + 1] = state.size-i+1
		end
	end
	return traversals
end

function game_valid_free_cell(state, cell)
	local size = state.size
	if cell.x > 0 and cell.x <= size and cell.y > 0 and cell.y <= size then
		return state.map[(cell.x-1)*size+cell.y] == 0
	end
	return false
end

function game_find_farthest_pos(state, cell, vector)
	local loc = {x= cell.x, y= cell.y}
	local prev
	repeat
		prev = loc
		loc = {x=prev.x + vector.x, y=prev.y + vector.y}
	until not game_valid_free_cell(state, loc)
	local nextid = (loc.x-1)*state.size+loc.y
	return prev, nextid
end

function game_move_available(state)
	for i=1,state.size do
		for j=1,state.size do
			local id = (i-1)*state.size+j
			local v = state.map[id]
			if v ~= 0 then
				for dir=1,4 do
					local vec = game_get_direction_vector(dir)
					local cell = {x= i+vec.x, y= j+vec.y}
					local tid = (cell.x-1)*state.size+cell.y
					local val = state.map[tid]
					if val ~= nil and v == val then
						return true
					end
				end
			end
		end
	end
	return false
end

function game_move(state)
	local vector = game_get_direction_vector(state.direction)
	local traversals = game_build_traversals(state, vector)
	local needmove = false
	for i=1,state.size*state.size do
		state.merged[i] = false
	end
	for _i=1,#traversals.x do
		local i = traversals.x[_i]
		for _j=1,#traversals.y do
			local j = traversals.y[_j]
			local cell = { x= i, y= j }
			local tileid = (cell.x-1)*state.size+cell.y
			local tileid2 = tileid
			local tile = state.map[tileid]
			if tile ~= 0 then
				local pos, nextid
				pos, nextid = game_find_farthest_pos(state, cell, vector)
				if state.map[nextid] == tile and not state.merged[nextid] then
					state.merged[nextid] = true
					state.map[tileid] = 0
					state.map[nextid] = tile + 1
					tileid = nextid
					state.score = state.score + math.ceil(2 ^ state.map[nextid])
					if tile > 10 and state.gamestate ~= state.gamestate_type.win then
						state.gamestate = state.gamestate_type.win
					end
				else
					state.map[tileid] = 0
					tileid = (pos.x-1)*state.size+pos.y
					state.map[tileid] = tile
				end
			end
			if tileid ~= tileid2 then
				needmove = true
			end
		end
	end
	if needmove then
		game_add_tile(state)
		local a = game_get_available_cells(state)
		if #a == 0 then
			if not game_move_available(state) then
				state.gamestate = state.gamestate_type.failed
			end
		end
		game_paint(state)
	end
end

return M